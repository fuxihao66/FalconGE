#pragma once
#include <memory>
#include <string>
#include "../Utility/utility.h"
// fxxking tedious register/layout


// TODO: execute之前 检查资源依赖关系（图）根据这个来插入barrier    所有的资源根据管线依赖关系构建一个有向图，如果某个资源被多个管线依赖，则会根据图的依赖关系自动进行转换
namespace Falcon {
    /*class Resource {
    public:
        using Ptr = std::shared_ptr<Resource>;
        ResourceState  _currentState;
    };*/


    

    class ResourceD3D12Impl {
    protected:
        ComPtr<ID3D12Resource> resource;
        ComPtr<ID3D12Resource> uploadBuffer;
        

        D3D12_RESOURCE_STATES _currentState;

    public:
        using Ptr = std::shared_ptr<ResourceD3D12Impl>;
        
        D3D12_RESOURCE_STATES GetCurrentState()const;
        void SetState(D3D12_RESOURCE_STATES s);

        ID3D12Resource* GetResourcePointer();
    };
    /*D3D12_CPU_DESCRIPTOR_HANDLE ResourceD3D12Impl::GetSRVDescriptorCPU() const {
        return cpuSRVDescriptorHandle;
    }
    D3D12_GPU_DESCRIPTOR_HANDLE ResourceD3D12Impl::GetSRVDescriptorGPU() const {
        return gpuSRVDescriptorHandle;
    }*/
    ID3D12Resource* ResourceD3D12Impl::GetResourcePointer() {
        return resource.Get();
    }

    D3D12_RESOURCE_STATES ResourceD3D12Impl::GetCurrentState() const {
        return _currentState;
    }
    void ResourceD3D12Impl::SetState(D3D12_RESOURCE_STATES s) {
        if (s != _currentState) {
            CD3DX12_RESOURCE_BARRIER barrier;
            barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), _currentState, s);
            RenderEngineD3D12Impl::Instance()->GetCmdList()->ResourceBarrier(1, &barrier);
        }
        _currentState = s;
    }


    
    /*class Buffer : public Resource {
    public:
        using Ptr = std::shared_ptr<Buffer>;

    };*/
    
    class BufferD3D12Impl : public ResourceD3D12Impl {

        /*D3D12_INDEX_BUFFER_VIEW _ibv;
        D3D12_VERTEX_BUFFER_VIEW _vbv;*/
        uint _bufferSize;
        uint _structSize;
        uint _elementNum;
    public:
        using Ptr = std::shared_ptr<BufferD3D12Impl>;


        //SetGraphicsRootDescriptorTable vs SetgraphicsRoot**View   前者需要descriptor  后者直接把资源的gpu pointer传过去就可以
        // 两种方法对应两种不同的parameter创建方式
        // 一个rootsignature只能有64个parameter，如果每个变量都创建一个parameter 则会很快消耗完  https://dench.flatlib.jp/d3d/d3d12/descriptor
        //https://zhuanlan.zhihu.com/p/73016473  vk对比dx

        BufferD3D12Impl() {

        }
        uint GetElementNum() {
            return _elementNum;
        }


        

        BufferD3D12Impl(uint numElement, uint structSize) {
            _bufferSize = numElement * structSize;
            _structSize = structSize;
            _elementNum = numElement;

            RenderEngineD3D12Impl::Instance()->GetD3D12Device()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(structSize * numElement),
                D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
                nullptr,
                IID_PPV_ARGS(&resource));

            RenderEngineD3D12Impl::Instance()->GetD3D12Device()->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(structSize * numElement),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&uploadBuffer));


            /*if (resourceFlag & ResourceUsageFlag::UAV) {

            }
            if (resourceFlag & ResourceUsageFlag::SRV) {

            }
            if (resourceFlag & ResourceUsageFlag::CBV) {

            }*/


        }
        void SetBlob(const void* pData, size_t length) {
            UINT8* mappedUploadHeap = nullptr;
            uploadBuffer->Map(0, &CD3DX12_RANGE(0, 0), reinterpret_cast<void**>(&mappedUploadHeap));

            memcpy(mappedUploadHeap, pData, length);

            uploadBuffer->Unmap(0, &CD3DX12_RANGE(0, 0));

            RenderEngineD3D12Impl::Instance()->GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, D3D12_RESOURCE_STATE_COPY_DEST));
            RenderEngineD3D12Impl::Instance()->GetCmdList()->CopyBufferRegion(resource.Get(), 0, uploadBuffer.Get(), 0, resource->GetDesc().Width);
            RenderEngineD3D12Impl::Instance()->GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
        }



    };

    //class Texture : public Resource {
    //public:
    //    using Ptr = std::shared_ptr<Texture>;
    //};

    class TextureD3D12Impl : public ResourceD3D12Impl{

    protected:
        uint _width, _height;


    public:
        using Ptr = std::shared_ptr<TextureD3D12Impl>;
        

        TextureD3D12Impl(const std::string& path, DXGI_FORMAT format, D3D12_RESOURCE_STATES initResourceState);
        TextureD3D12Impl(int w, int h, DXGI_FORMAT format, D3D12_RESOURCE_STATES initResourceState);

    };
    TextureD3D12Impl::TextureD3D12Impl(const std::string& path, DXGI_FORMAT format, D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {

        std::vector<UINT8> texture = GenerateTextureData();



        // Create the output resource. The dimensions and format should match the swap-chain.
        auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, w, h, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        RenderEngineD3D12Impl::Instance()->GetD3D12Device()->CreateCommittedResource(
            &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&resource));

        _currentState = initResourceState;



        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(resource.Get(), 0, 1);

        // Create the GPU upload buffer.
        RenderEngineD3D12Impl::Instance()->GetD3D12Device()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadBuffer));


        D3D12_SUBRESOURCE_DATA textureData = {};
        textureData.pData = &texture[0];
        textureData.RowPitch = TextureWidth * TexturePixelSize;
        textureData.SlicePitch = textureData.RowPitch * TextureHeight;

        UpdateSubresources(RenderEngineD3D12Impl::Instance()->GetCmdList().Get(), resource.Get(), uploadBuffer.Get(), 0, 0, 1, &textureData);
        RenderEngineD3D12Impl::Instance()->GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, initResourceState));

        
    }
    TextureD3D12Impl::TextureD3D12Impl(int w, int h, DXGI_FORMAT format,D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
    {
        // Create the output resource. The dimensions and format should match the swap-chain.
        auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, w, h, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS | D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        RenderEngineD3D12Impl::Instance()->GetD3D12Device()->CreateCommittedResource(
            &defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, initResourceState, nullptr, IID_PPV_ARGS(&resource));

        _currentState = initResourceState;
        
        

    }

   

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








