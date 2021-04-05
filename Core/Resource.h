
#include <memory>
#include <string>
#include "../Utility/utility.h"
// fxxking tedious register/layout


// TODO: execute之前 检查资源依赖关系（图）根据这个来插入barrier    所有的资源根据管线依赖关系构建一个有向图，如果某个资源被多个管线依赖，则会根据图的依赖关系自动进行转换
namespace Falcon {
    class Resource {
    public:
        using Ptr = std::shared_ptr<Resource>;
        ResourceState  _currentState;
    };


    

    class ResourceD3D12Impl : public Resource {
    protected:
        ComPtr<ID3D12Resource> resource;
        ComPtr<ID3D12Resource> uploadBuffer;
        

        D3D12_RESOURCE_STATES state;

    public:
        using Ptr = std::shared_ptr<ResourceD3D12Impl>;
        
        ResourceState GetCurrentState()const;
        void SetState(D3D12_RESOURCE_STATES s);


    };
    /*D3D12_CPU_DESCRIPTOR_HANDLE ResourceD3D12Impl::GetSRVDescriptorCPU() const {
        return cpuSRVDescriptorHandle;
    }
    D3D12_GPU_DESCRIPTOR_HANDLE ResourceD3D12Impl::GetSRVDescriptorGPU() const {
        return gpuSRVDescriptorHandle;
    }*/
    ResourceState ResourceD3D12Impl::GetCurrentState() const {
        return _currentState;
    }
    void ResourceD3D12Impl::SetState(D3D12_RESOURCE_STATES s) {
        if (s != state) {
            CD3DX12_RESOURCE_BARRIER barrier;
            barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), state, s);
            _commandList->ResourceBarrier(1, &barrier);
        }
        state = s;
    }


    
    class Buffer : public Resource {
    public:
        using Ptr = std::shared_ptr<Buffer>;

    };
    
    class BufferD3D12Impl : public ResourceD3D12Impl, public Buffer {


    public:
        using Ptr = std::shared_ptr<BufferD3D12Impl>;


        //SetGraphicsRootDescriptorTable vs SetgraphicsRoot**View   前者需要descriptor  后者直接把资源的gpu pointer传过去就可以
        // 两种方法对应两种不同的parameter创建方式
        // 一个rootsignature只能有64个parameter，如果每个变量都创建一个parameter 则会很快消耗完  https://dench.flatlib.jp/d3d/d3d12/descriptor
        //https://zhuanlan.zhihu.com/p/73016473  vk对比dx


        static BufferD3D12Impl::Ptr Create(uint structSize, uint numElement) {
            

            ThrowIfFailed(m_device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
                D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                nullptr,
                IID_PPV_ARGS(&m_vertexBuffer)));

            ThrowIfFailed(m_device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_vertexBufferUpload)));


            /*if (resourceFlag & ResourceUsageFlag::UAV) {

            }
            if (resourceFlag & ResourceUsageFlag::SRV) {

            }
            if (resourceFlag & ResourceUsageFlag::CBV) {

            }*/


        }
        void SetBlob(const void* pData, size_t length) {
            UINT8* mappedUploadHeap = nullptr;
            ThrowIfFailed(uploadBuffer->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&mappedUploadHeap)));

             memcpy(mappedUploadHeap, pData, length);

            uploadBuffer->Unmap(0, &CD3DX12_RANGE(0, 0));


            m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST));
            m_commandList->CopyBufferRegion(resource.Get(), 0, uploadBuffer.Get(), 0, resource->GetDesc().Width);
            m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
        }



    };

    class Texture : public Resource {
    public:
        using Ptr = std::shared_ptr<Texture>;
    };
    // TODO: texture同时可能是srv 也可能是uav

    class TextureD3D12Impl : public ResourceD3D12Impl, public Texture {

    protected:
        uint _width, _height;


    public:
        using Ptr = std::shared_ptr<TextureD3D12Impl>;
        

        void CreateTextureFromFile(const std::string& path);
        void CreateTexture(int w, int h, format);

    };
    void TextureD3D12Impl::CreateTexture(int w, int h, format){
        // Create the output resource. The dimensions and format should match the swap-chain.
        auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, w, h, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

        auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        ThrowIfFailed(device->CreateCommittedResource(
            &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_raytracingOutput)));

        /*if (resourceFlag & ResourceUsageFlag::UAV) {

            D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
            m_raytracingOutputResourceUAVDescriptorHeapIndex = AllocateDescriptor(&uavDescriptorHandle, m_raytracingOutputResourceUAVDescriptorHeapIndex);
            D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
            UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
            device->CreateUnorderedAccessView(m_raytracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
            m_raytracingOutputResourceUAVGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_raytracingOutputResourceUAVDescriptorHeapIndex, m_descriptorSize);
        }
        if (resourceFlag & ResourceUsageFlag::SRV) {

        }*/
        

    }

    //// 用户需要自行拆分
    //class ResourceBinding {
    //    std::map<std::string, Resource::Ptr> _binding;
    //    // 通过初始化列表  _binding = {{"g_Diffuse", DiffuseTexturePtr}, {}};

    //    void Create(BaseGraphicsPass:Ptr renderPass) {
    //        auto shaderRegisterSpace = renderPass->GetShaderRegisterSpace();

    //        auto 
    //    }

    //    void Bind() {
    //        SetGraphicsRootDescriptorTable();
    //    }
    //};

}




//class Texture : public Resource{
//
//};

// class TextureVKImpl: public Resource{

// protected:

//     VkDescriptorImageInfo GetImageDescriptor();
// };


// class RenderTargetVKImpl: public TextureVKImpl{

// private:
// };








