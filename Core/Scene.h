#pragma once
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
        DirectX::XMMATRIX _modelMat;

        DirectX::XMMATRIX GetModelMat() {
            return _modelMat;
        }

        BufferD3D12Impl::Ptr _vertexBuffer;
        BufferD3D12Impl::Ptr _indexBuffer;

        D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer() {
            D3D12_VERTEX_BUFFER_VIEW vbv;

            vbv.BufferLocation = _vertexBuffer->GetResourcePointer()->GetGPUVirtualAddress();
            vbv.SizeInBytes = _indexBuffer->GetElementNum() * sizeof(FSVertex);
            vbv.StrideInBytes = sizeof(FSVertex);

            return vbv;
        }
        D3D12_INDEX_BUFFER_VIEW GetIndexBuffer() {

            D3D12_INDEX_BUFFER_VIEW ibv;
            ibv.BufferLocation = _indexBuffer->GetResourcePointer()->GetGPUVirtualAddress();
            ibv.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
            ibv.SizeInBytes = _indexBuffer->GetElementNum() * sizeof(uint);

            return ibv;

        }
        uint GetIndexBufferSize(){
            return _indexBuffer->GetElementNum();
        }
        uint GetVertexNum() {
            return _vertexBuffer->GetElementNum();
        }
    };

    struct Material {
        string _matName;
        using Ptr = std::shared_ptr<Material>;


        TextureD3D12Impl::Ptr _diffuse;
        TextureD3D12Impl::Ptr _specular;
        TextureD3D12Impl::Ptr _normal;
        TextureD3D12Impl::Ptr _roughness;

        ShaderResourceBindingD3D12Impl::Ptr _srb;

        Material() {

        }

        Material(const string& matName, const string& df, const string& sf, const string& nf, const string& rf) {
            _matName = matName;
            _diffuse = Factory::CreateTextureFromFile(df, _matName+"_diffuse");
            _specular = Factory::CreateTextureFromFile(sf, _matName + "_specular");
            _normal = Factory::CreateTextureFromFile(nf, _matName + "_normal");
            _roughness = Factory::CreateTextureFromFile(rf, _matName + "_roughness");
        }

        void BuildResourceBinding() {
            if (_srb == nullptr)
                return;
            auto mapping = std::map<std::string, ResourceD3D12Impl::Ptr>{ {"gDiffuse", _diffuse},{"gSpecular", _specular},{"gNormal", _normal},{"gRoughness", _roughness} };

            _srb->BindResource(mapping);
        }

        ShaderResourceBindingD3D12Impl::Ptr GetResourceBinding() {
            return _srb;
        }
    };
    class Camera {
    private:
        DirectX::XMFLOAT3 _pos;
        DirectX::XMFLOAT3 _up;
        DirectX::XMFLOAT3 _lookat;

        float _fov = 1.5f;
        float _aspectRatio;
        float _near = 0.1f;
        float _far = 5000.0f;
    public:
        using Ptr = std::shared_ptr<Camera>;
        Camera() {
            _aspectRatio = RenderEngineD3D12Impl::Instance()->GetWidthHeight().x / (float)(RenderEngineD3D12Impl::Instance()->GetWidthHeight().y);
        }
        DirectX::XMFLOAT3 GetPosW() {
            return _pos;
        }
        DirectX::XMMATRIX GetViewMat() {
            return DirectX::XMMatrixLookToRH(XMLoadFloat3(&_pos), XMLoadFloat3(&_lookat), XMLoadFloat3(&_up));
        }

        DirectX::XMMATRIX GetProjMat() {
            XMMatrixPerspectiveFovRH(_fov, _aspectRatio, _near, _far);
        }
    };
    class Scene {
        //  �ṩ������Ľӿ� ����mesh����Ⱦ
        // ͬʱ�ṩ�ӿ�����ȡscene������mesh��buffer����������rt���龰�У�
    private:
        Camera::Ptr _camera;
        uint _numMeshInstance;
        uint _numMaterial;


        vector<MeshInstanceData> _meshWrapping;
        vector<Mesh::Ptr> _mesh;
        vector<Material::Ptr> _material;
        /*VertexBuffer::Ptr GetVertexBuffer();
        IndexBuffer::Ptr GetIndexBuffer();*/
        ShaderObject::Ptr _shaderObj;
    public:
        using Ptr = shared_ptr<Scene>;
        
        Scene() {

            // ����rt���ٽṹ 
            BuildAccelerationStructures();

        }
        
        void BuildAccelerationStructures()
        {
            //auto device = m_deviceResources->GetD3DDevice();
            //auto commandList = m_deviceResources->GetCommandList();
            //auto commandQueue = m_deviceResources->GetCommandQueue();
            //auto commandAllocator = m_deviceResources->GetCommandAllocator();

            //// Reset the command list for the acceleration structure construction.
            //commandList->Reset(commandAllocator, nullptr);

            //D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
            //geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
            //geometryDesc.Triangles.IndexBuffer = m_indexBuffer.resource->GetGPUVirtualAddress();
            //geometryDesc.Triangles.IndexCount = static_cast<UINT>(m_indexBuffer.resource->GetDesc().Width) / sizeof(Index);
            //geometryDesc.Triangles.IndexFormat = DXGI_FORMAT_R16_UINT;
            //geometryDesc.Triangles.Transform3x4 = 0;
            //geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            //geometryDesc.Triangles.VertexCount = static_cast<UINT>(m_vertexBuffer.resource->GetDesc().Width) / sizeof(Vertex);
            //geometryDesc.Triangles.VertexBuffer.StartAddress = m_vertexBuffer.resource->GetGPUVirtualAddress();
            //geometryDesc.Triangles.VertexBuffer.StrideInBytes = sizeof(Vertex);

            //// Mark the geometry as opaque. 
            //// PERFORMANCE TIP: mark geometry as opaque whenever applicable as it can enable important ray processing optimizations.
            //// Note: When rays encounter opaque geometry an any hit shader will not be executed whether it is present or not.
            //geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;

            //// Get required sizes for an acceleration structure.
            //D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;

            //D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};
            //D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& bottomLevelInputs = bottomLevelBuildDesc.Inputs;
            //bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            //bottomLevelInputs.Flags = buildFlags;
            //bottomLevelInputs.NumDescs = 1;
            //bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
            //bottomLevelInputs.pGeometryDescs = &geometryDesc;

            //D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
            //D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
            //topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            //topLevelInputs.Flags = buildFlags;
            //topLevelInputs.NumDescs = 1;
            //topLevelInputs.pGeometryDescs = nullptr;
            //topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

            //D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
            //m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
            //ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

            //D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO bottomLevelPrebuildInfo = {};
            //m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &bottomLevelPrebuildInfo);
            //ThrowIfFalse(bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

            //ComPtr<ID3D12Resource> scratchResource;
            //AllocateUAVBuffer(device, max(topLevelPrebuildInfo.ScratchDataSizeInBytes, bottomLevelPrebuildInfo.ScratchDataSizeInBytes), &scratchResource, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, L"ScratchResource");

            //// Allocate resources for acceleration structures.
            //// Acceleration structures can only be placed in resources that are created in the default heap (or custom heap equivalent). 
            //// Default heap is OK since the application doesn�t need CPU read/write access to them. 
            //// The resources that will contain acceleration structures must be created in the state D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, 
            //// and must have resource flag D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS. The ALLOW_UNORDERED_ACCESS requirement simply acknowledges both: 
            ////  - the system will be doing this type of access in its implementation of acceleration structure builds behind the scenes.
            ////  - from the app point of view, synchronization of writes/reads to acceleration structures is accomplished using UAV barriers.
            //{
            //    D3D12_RESOURCE_STATES initialResourceState = D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE;

            //    AllocateUAVBuffer(device, bottomLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_bottomLevelAccelerationStructure, initialResourceState, L"BottomLevelAccelerationStructure");
            //    AllocateUAVBuffer(device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");
            //}

            //// Create an instance desc for the bottom-level acceleration structure.
            //ComPtr<ID3D12Resource> instanceDescs;
            //D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};
            //instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;
            //instanceDesc.InstanceMask = 1;
            //instanceDesc.AccelerationStructure = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
            //AllocateUploadBuffer(device, &instanceDesc, sizeof(instanceDesc), &instanceDescs, L"InstanceDescs");

            //// Bottom Level Acceleration Structure desc
            //{
            //    bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
            //    bottomLevelBuildDesc.DestAccelerationStructureData = m_bottomLevelAccelerationStructure->GetGPUVirtualAddress();
            //}

            //// Top Level Acceleration Structure desc
            //{
            //    topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
            //    topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress();
            //    topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();
            //}

            //auto BuildAccelerationStructure = [&](auto* raytracingCommandList)
            //{
            //    raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
            //    commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure.Get()));
            //    raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
            //};

            //// Build acceleration structure.
            //BuildAccelerationStructure(m_dxrCommandList.Get());

            //// Kick off acceleration structure construction.
            //m_deviceResources->ExecuteCommandList();

            //// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
            //m_deviceResources->WaitForGpu();
        }
        void CreatePassFromFile(const std::string& vsFilename, const std::string& psFileName, const std::string& passName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st, uint numRenderTarget) {
            // ��ͳ
            _shaderObj = std::make_shared<ShaderObjectVsPs>(vsFilename, psFileName, passName, entryPoint, sm, st, numRenderTarget);

            BuildResourceBinding();
            
        }

        /*void SetSceneRenderPass(std::string& filePath) {

        }*/
        void BuildResourceBinding() {
            for (auto matPtr : _material) {
                matPtr->_srb = _shaderObj->CreateShaderResourceBinding();

                matPtr->BuildResourceBinding();
            }
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


        BufferD3D12Impl::Ptr _viewProjResource;
        BufferD3D12Impl::Ptr _modelResource;

        DirectX::XMFLOAT3 GetCameraPosW() {
            return _camera->GetPosW();
        }
        Camera::Ptr GetCamera() {
            return _camera;
        }


        ShaderObject::Ptr GetShaderObject() {
            return _shaderObj;
        }

        // rt ��Դ������
        void RenderScene(uint numAttachments, RenderTargetD3D12Impl::Ptr Fbo) {
            auto tempMat = XMMatrixMultiply(_camera->GetViewMat(), _camera->GetProjMat());
            _viewProjResource->SetBlob(&tempMat, sizeof(tempMat));
            for (uint32_t i = 0; i < _numMeshInstance; i++)
            {
                const MeshInstanceData& inst = GetMeshInstance(i);
                auto mesh = GetMesh(inst.meshID);
                auto mat = GetMaterial(inst.materialID);

                _modelResource->SetBlob(&mesh->GetModelMat(), sizeof(mesh->GetModelMat()));

                RenderEngineD3D12Impl::Instance()->BindResourceBindingToGraphicsPipeline(mat->GetResourceBinding());

                RenderEngineD3D12Impl::Instance()->BindConstantToGraphicsPipeline("gModelMat", _modelResource);// ֻ��mvp����   ��֧��������
                RenderEngineD3D12Impl::Instance()->BindConstantToGraphicsPipeline("gViewProjMat", _viewProjResource);// ֻ��mvp����   ��֧��������



                RenderEngineD3D12Impl::Instance()->SetViewports();
                RenderEngineD3D12Impl::Instance()->SetScissorRects();
                RenderEngineD3D12Impl::Instance()->SetRenderTargets(numAttachments, Fbo);

                RenderEngineD3D12Impl::Instance()->SetVertexBuffers(mesh->GetVertexBuffer());
                RenderEngineD3D12Impl::Instance()->SetIndexBuffers(mesh->GetIndexBuffer());

                RenderEngineD3D12Impl::Instance()->SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

                RenderEngineD3D12Impl::Instance()->DrawInstanced(mesh->GetIndexBufferSize(), 1);
            }
            
        }

        // �������û��󶨵�srv , ray tracing��ʱ������
        ResourceD3D12Impl::Ptr GetSceneMeshBuffer() {
            return nullptr;
        }
        void TraceScene() {

        }
    };
}
