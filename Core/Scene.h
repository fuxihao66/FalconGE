#include "RenderEngine.h"

using std::vector;
using std::map;
using std::shared_ptr;
namespace Falcon {
    struct MeshInstanceData {
        uint meshID;
        uint materialID;
    };

    struct Mesh{
        using Ptr = std::shared_ptr<Mesh>;
        VertexBuffer GetVertexBuffer();
        IndexBuffer GetIndexBuffer();
    };

    struct Material {

        using Ptr = std::shared_ptr<Material>;

        ShaderResourceBinding::Ptr _srb;
    };
    class Camera {
    private:
        DirectX::XMFLOAT3 _pos;
        DirectX::XMFLOAT3 _up;
        DirectX::XMFLOAT3 _lookat;
    public:
        using Ptr = std::shared_ptr<Camera>;
        DirectX::XMFLOAT3 GetPosW() {
            return _pos;
        }
    };
    class Scene {
        //  提供最基本的接口 进行mesh的渲染
        // 同时提供接口来获取scene中所有mesh的buffer（可以用于rt等情景中）
    private:
        Camera::Ptr _camera;
        uint _numMeshInstance;
        uint _numMaterial;


        vector<MeshInstanceData> _meshWrapping;
        vector<Mesh::Ptr> _mesh;
        vector<Material::Ptr> _material;
        /*VertexBuffer::Ptr GetVertexBuffer();
        IndexBuffer::Ptr GetIndexBuffer();*/
    public:
        using Ptr = shared_ptr<Scene>;
        void BuildAccelerationStructures()
        {
            auto device = m_deviceResources->GetD3DDevice();
            auto commandList = m_deviceResources->GetCommandList();
            auto commandQueue = m_deviceResources->GetCommandQueue();
            auto commandAllocator = m_deviceResources->GetCommandAllocator();

            // Reset the command list for the acceleration structure construction.
            commandList->Reset(commandAllocator, nullptr);

            D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
            geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
            geometryDesc.Triangles.IndexBuffer = m_indexBuffer.resource->GetGPUVirtualAddress();
            geometryDesc.Triangles.IndexCount = static_cast<UINT>(m_indexBuffer.resource->GetDesc().Width) / sizeof(Index);
            geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
            geometryDesc.Triangles.Transform3x4 = 0;
            geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            geometryDesc.Triangles.VertexCount = static_cast<UINT>(m_vertexBuffer.resource->GetDesc().Width) / sizeof(Vertex);
            geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer.resource->GetGPUVirtualAddress();
            geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

            // Mark the geometry as opaque. 
            // PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
            // Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
            geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

            // Get required sizes for an acceleration structure.
            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = bottomLevelBuildDesc.Inputs;
            bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            bottomLevelInputs.Flags = buildFlags;
            bottomLevelInputs.NumDescs = 1;
            bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
            bottomLevelInputs.pGeometryDescs = &geometryDesc;

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
            topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            topLevelInputs.Flags = buildFlags;
            topLevelInputs.NumDescs = 1;
            topLevelInputs.pGeometryDescs = nullptr;
            topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
            m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
            ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
            m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
            ThrowIfFalse(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

            ComPtr<ID3D12Resource> scratchResource;
            AllocateUAVBuffer(device, max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes), &scratchResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

            // Allocate resources for acceleration structures.
            // Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
            // Default heap is OK since the application doesn抰 need CPU read/write access to them. 
            // The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
            // and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
            //  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
            //  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
            {
                D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

                AllocateUAVBuffer(device, bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_bottomLevelAccelerationStructure, initialResourceState, L"BottomLevelAccelerationStructure");
                AllocateUAVBuffer(device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");
            }

            // Create an instance desc for the bottom-level acceleration structure.
            ComPtr<ID3D12Resource> instanceDescs;
            D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
            instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
            instanceDesc.InstanceMask = 1;
            instanceDesc.AccelerationStructure = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
            AllocateUploadBuffer(device, &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");

            // Bottom Level Acceleration Structure desc
            {
                bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
                bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
            }

            // Top Level Acceleration Structure desc
            {
                topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
                topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
                topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
            }

            auto BuildAccelerationStructure = [&](auto* raytracingCommandList)
            {
                raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
                commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure.Get()));
                raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
            };

            // Build acceleration structure.
            BuildAccelerationStructure(m_dxrCommandList.Get());

            // Kick off acceleration structure construction.
            m_deviceResources->ExecuteCommandList();

            // Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
            m_deviceResources->WaitForGpu();
        }
        void CreateFromFile() {
            // 传统



            // 构建rt加速结构 
            BuildAccelerationStructures();
        }

        void SetSceneRenderPass(std::string& filePath) {

        }
        void BuildResourceBinding() {

        }

        MeshInstanceData GetMeshInstance(uint i) {
            return _meshWrapping[i];
        }

        Mesh::Ptr GetMesh(uint meshId) {
            return _mesh[meshId];

        }

        Material::Ptr GetMaterial(uint materialId) {
            return _material[materialId];
        }


        Resource::Ptr _viewProjResource;
        Resource::Ptr _modelResource;

        DirectX::XMFLOAT3 GetCameraPosW() {
            return _camera->GetPosW();
        }
        Camera::Ptr GetCamera() {
            return _camera;
        }

        // rt 资源创建等
        void RenderScene(RenderTarget::Ptr Fbo) {

            _viewProjResource->SetBlob(_camera->ViewMat() * RenderEngine::Instance()->GetProjMat());
            for (uint32_t i = 0; i < _numMeshInstance; i++)
            {
                const MeshInstanceData& inst = GetMeshInstance(i);
                auto mesh = GetMesh(inst.meshID);
                auto mat = GetMaterial(inst.materialID);

                _modelResource->SetBlob(mesh->ModelMat());

                RenderEngine::Instance()->BindResourceBindingToPipeline(mat->ResourceBinding());

                RenderEngine::Instance()->BindConstant("gModelMat", _modelResource->VirtualGPUAddress);// 只有mvp矩阵   不支持其他的
                RenderEngine::Instance()->BindConstant("gViewProjMat", _viewProjResource->VirtualGPUAddress);// 只有mvp矩阵   不支持其他的


                RenderEngine::Instance()->SetRenderTarget(Fbo);

                Falcon::RenderEngine::Instance()->BindInputBindVertex(mesh->GetVertexBuffer());
                Falcon::RenderEngine::Instance()->BindInputBindIndex(mesh->GetIndexBuffer());

                Falcon::RenderEngine::Instance()->BindInputSetTopology(TOPOLOGY_TYPE::TRIANGLE);

                Falcon::RenderEngine::Instance()->Draw();
            }
            
        }

        // 可以让用户绑定到srv 
        Resource::Ptr GetSceneMeshBuffer() {

        }
        void TraceScene() {

        }
    };
}
