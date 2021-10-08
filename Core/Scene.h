#pragma once
#include "RenderEngine.h"

using std::vector;
using std::map;
using std::shared_ptr;
namespace Falcon {

    struct InstanceStride{
        uint VertexStride;
        uint IndexStride;
    };

    struct MeshInstanceData {
        std::string MeshKey;
        std::string MaterialKey;
        DirectX::XMMATRIX ModelMat;
        InstanceStride Stride;
    };

    struct Mesh{
        using Ptr = std::shared_ptr<Mesh>;

        // DirectX::XMMATRIX GetModelMat() {
        //     return _modelMat;
        // }

        // BufferD3D12Impl::Ptr _vertexBuffer;
        // BufferD3D12Impl::Ptr _indexBuffer;

        // D3D12_VERTEX_BUFFER_VIEW GetVertexBuffer() {
        //     D3D12_VERTEX_BUFFER_VIEW vbv;

        //     vbv.BufferLocation = _vertexBuffer->GetResourcePointer()->GetGPUVirtualAddress();
        //     vbv.SizeInBytes = _indexBuffer->GetElementNum() * sizeof(FSVertex);
        //     vbv.StrideInBytes = sizeof(FSVertex);

        //     return vbv;
        // }
        // D3D12_INDEX_BUFFER_VIEW GetIndexBuffer() {

        //     D3D12_INDEX_BUFFER_VIEW ibv;
        //     ibv.BufferLocation = _indexBuffer->GetResourcePointer()->GetGPUVirtualAddress();
        //     ibv.Format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
        //     ibv.SizeInBytes = _indexBuffer->GetElementNum() * sizeof(uint);

        //     return ibv;

        // }

        void BuildBLAS(){
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
               
            }

            // 每个BLAS对应一个instanceDesc
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


            raytracingCommandList->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);
            commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(m_bottomLevelAccelerationStructure.Get()));

            // 要用专门的command list ?
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

        float _fov = PI / 2.0;
        float _aspectRatio;
        float _near = 0.1f;
        float _far = 5000.0f;
    public:
        using Ptr = std::shared_ptr<Camera>;
        Camera() {
            _aspectRatio = RenderEngineD3D12Impl::Instance()->GetWidthHeight().x / (float)(RenderEngineD3D12Impl::Instance()->GetWidthHeight().y);
        }
        void SetPosition(float x, float y, float z){
            _pos = DirectX::XMFLOAT3(x, y, z);
        }
        void SetUpVec(float x, float y, float z){
            _up = DirectX::XMFLOAT3(x, y, z);
        }
        void SetLookAt(float x, float y, float z){
            _lookat = DirectX::XMFLOAT3(x, y, z);
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


    // TODO: 
    // 1. srv  vertex和index，stride 和instance2material,  
    // 2. rt   每个geometry一个blas，然后多个instance引用同一个geometry（blas）需要创建多个instance，每个instance下面直接用primitive index索引三角形
    // 3. material srb
    // 4. emitter to mesh
    // 5. 纹理（albedo等）需要localRootSignature  把descriptor绑定到hit group对应的SBT上
    // 想办法封装起来  用户直接通过MaterialAttribute来访问
        // 第一个是shaderIdentifier  后边是对应的localRootSignature定义的parameter    
        // for (UINT i = 0; i < numMeshes; i++)
        // {
        //     byte *pShaderRecord = i * shaderRecordSizeInBytes + pShaderTable;
        //     memcpy(pShaderRecord, pHitGroupIdentifierData, shaderIdentifierSize);

        //     UINT materialIndex = model.m_pMesh[i].materialIndex;
        //     memcpy(pShaderRecord + offsetToDescriptorHandle, &g_GpuSceneMaterialSrvs[materialIndex].ptr, sizeof(g_GpuSceneMaterialSrvs[materialIndex].ptr));

        //     MaterialRootConstant material;
        //     material.MaterialID = i;
        //     memcpy(pShaderRecord + offsetToMaterialConstants, &material, sizeof(material));
        // }


    class Scene {
        //  提供最基本的接口 进行mesh的渲染
        // 提供一个完整的vertex buffer和indices buffer
        // 每个mesh渲染的时候，通过offset来得到对应的vertex buffer和indices buffer的handle
    private:
        Camera::Ptr _camera;
        uint _numMeshInstance;
        uint _numMaterial;

        // 维护一个大buffer和stride array
        BufferD3D12Impl::Ptr _vertexBuffer;
        BufferD3D12Impl::Ptr _indexBuffer;
        
        // 每次只需要绑定一个完整的buffer

        vector<MeshInstanceData> _instance;
        map<string, Mesh::Ptr> _mesh;
        map<string, Material::Ptr> _material;
        /*VertexBuffer::Ptr GetVertexBuffer();
        IndexBuffer::Ptr GetIndexBuffer();*/
        ShaderObject::Ptr _shaderObj;
        string _passName;


        


        ResourceD3D12Impl::Ptr _as;
    public:
        using Ptr = shared_ptr<Scene>;
        
        Scene() {

            // 构建rt加速结构 
            BuildAccelerationStructures();

        }
        

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

        ResourceD3D12Impl::Ptr GetAccelerationStruct(){
            return _as;
        }

        static shared_ptr<Scene> CreateSceneFromFile(const std::string& sceneFilePath){
            if (sceneFilePath.substr(sceneFilePath.size() - 3, 3) != "xml"){
                // error
                return nullptr;
            }



            shared_ptr<Scene> newScene = make_shared<Scene>();

            XMLDocument doc;
            doc.LoadFile( sceneFilePath );

            auto globalTextureNode = doc.FirstChildElement( "GlobalTexture" );

            auto geometryNode = doc.FirstChildElement( "Geometry" );
            auto materialNode = doc.FirstChildElement( "Material" );


            {
                auto instanceNode = doc.FirstChildElement( "Instance" );
                int index = 0;
                while(){
                    MeshInstance meshInst;

                    meshInst.MaterialKey = ;
                    meshInst.MeshKey = ;
                    for (int i = 0; i < 3; i++){
                        for (int j = 0; j < 4; j++){
                            meshInst.ModelMat[][] = ;
                        }
                    }
                    if (index > 0){
                        meshInst.Stride.VertexStride = ;
                        meshInst.Stride.IndexStride = ;
                    }
                    _instance.push_back(meshInst);

                    index += 1;
                }
            }


            {
                // 创建emitter to mesh
                // 对于某些rt 直接光照计算需要用到
                auto lightNode = doc.FirstChildElement( "Light" );


                D3D12Buffer<uint2> emitterToMesh;  // 每个元素代表一个light，uint2(i,j)第一个分量表示第i个mesh，第二个分量表示第j个三角形  用于获取某个三角形的顶点坐标
                D3D12Buffer<float3> emission;  
            }



            {
                auto cameraNode = doc.FirstChildElement( "Camera" );

                auto posNode = cameraNode.FirstChildElement( "Position" );
                auto lookatNode = cameraNode.FirstChildElement( "LookAt" );
                auto upNode = cameraNode.FirstChildElement( "UpVec" );

                newScene->_camera = std::make_shared<Camera>();
                newScene->_camera->SetPosition(atof(posNode.FirstChildElement("x").GetText()), atof(posNode.FirstChildElement("y").GetText()), atof(posNode.FirstChildElement("z").GetText()));
                newScene->_camera->SetLookAt(atof(lookatNode.FirstChildElement("x").GetText()), atof(lookatNode.FirstChildElement("y").GetText()), atof(lookatNode.FirstChildElement("z").GetText()));
                newScene->_camera->SetUpVec(atof(upNode.FirstChildElement("x").GetText()), atof(upNode.FirstChildElement("y").GetText()), atof(upNode.FirstChildElement("z").GetText()));

            }




        }

        void BuildAccelerationStructures()
        {

            
            // 每个geometry构建一个blas
           

            

            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
            D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& topLevelInputs = topLevelBuildDesc.Inputs;
            topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
            topLevelInputs.Flags = buildFlags;
            topLevelInputs.NumDescs = 1;
            topLevelInputs.pGeometryDescs = nullptr;
            topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;

            D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO topLevelPrebuildInfo = {};
            m_dxrDevice->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &topLevelPrebuildInfo);
            //ThrowIfFalse(topLevelPrebuildInfo.ResultDataMaxSizeInBytes > 0);

            
            AllocateUAVBuffer(device, topLevelPrebuildInfo.ResultDataMaxSizeInBytes, &m_topLevelAccelerationStructure, initialResourceState, L"TopLevelAccelerationStructure");
            

            
            


            // Top Level Acceleration Structure desc
            {
               topLevelBuildDesc.DestAccelerationStructureData = m_topLevelAccelerationStructure->GetGPUVirtualAddress();
               topLevelBuildDesc.ScratchAccelerationStructureData = scratchResource->GetGPUVirtualAddress(); // 临时空间？？
               topLevelBuildDesc.Inputs.InstanceDescs = instanceDescs->GetGPUVirtualAddress();    // 每个blas的desc  内存上连续  std::vector<D3D12_RAYTRACING_INSTANCE_DESC>& instanceDescs
               // desc里面
            } 

            auto BuildTLAS = [&](auto* raytracingCommandList)
            {
               
               raytracingCommandList->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);
            };

            //// Build acceleration structure.
            BuildTLAS(m_dxrCommandList.Get());

            //// Kick off acceleration structure construction.
            m_deviceResources->ExecuteCommandList();

            //// Wait for GPU to finish as the locally created temporary GPU resources will get released once we go out of scope.
            //m_deviceResources->WaitForGpu();
            RenderEngineD3D12Impl::Instance()->Flush();


            
        }
        void CreatePassFromFile(const std::string& vsFilename, const std::string& psFileName, const std::string& passName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st, uint numRenderTarget) {

            _passName = passName;
            // 传统
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
            return _instance[i];
        }

        Mesh::Ptr GetMesh(const std::string& meshId) {
            return _mesh[meshId];

        }

        Material::Ptr GetMaterial(const std::string&  materialId) {
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

        // rt 资源创建等
        void RenderScene(uint numAttachments, RenderTargetD3D12Impl::Ptr Fbo) {

            RenderEngineD3D12Impl::Instance()->SetPipeline(_passName);



            auto tempMat = XMMatrixMultiply(_camera->GetViewMat(), _camera->GetProjMat());
            _viewProjResource->SetBlob(&tempMat, sizeof(tempMat));
            for (uint32_t i = 0; i < _numMeshInstance; i++)
            {
                const MeshInstanceData& inst = GetMeshInstance(i);
                auto mesh = GetMesh(inst.meshID);
                auto mat = GetMaterial(inst.materialID);

                _modelResource->SetBlob(&mesh->GetModelMat(), sizeof(mesh->GetModelMat()));

                RenderEngineD3D12Impl::Instance()->BindResourceBindingToGraphicsPipeline(mat->GetResourceBinding());

                RenderEngineD3D12Impl::Instance()->BindConstantToGraphicsPipeline("gModelMat_"+_shaderObj->GetShaderName(), _modelResource);// 只有mvp矩阵   不支持其他的
                RenderEngineD3D12Impl::Instance()->BindConstantToGraphicsPipeline("gViewProjMat_"+_shaderObj->GetShaderName(), _viewProjResource);// 只有mvp矩阵   不支持其他的



                RenderEngineD3D12Impl::Instance()->SetViewports();
                RenderEngineD3D12Impl::Instance()->SetScissorRects();
                RenderEngineD3D12Impl::Instance()->SetRenderTargets(numAttachments, Fbo);

                RenderEngineD3D12Impl::Instance()->SetVertexBuffers(mesh->GetVertexBuffer());
                RenderEngineD3D12Impl::Instance()->SetIndexBuffers(mesh->GetIndexBuffer());

                RenderEngineD3D12Impl::Instance()->SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


                auto indexOffset = _indexStride[i];
                auto vertexOffset = _vertexStride[i];
                RenderEngineD3D12Impl::Instance()->DrawInstanced(mesh->GetIndexBufferSize(), 1, indexOffset, vertexOffset);
            }
            
        }

        // 可以让用户绑定到srv , ray tracing的时候有用
        ResourceD3D12Impl::Ptr GetSceneMeshBuffer() {
            return nullptr;
        }
        void TraceScene() {

        }
    };
}
