#pragma once
#include <string>
#include <memory>
#include "ScenePass.h"
#include "../Core/RenderEngine.h"
#include "../Core/Resource.h"
#include "../Core/RenderFactory.h"
namespace Falcon {

    class FullscreenPass : public BaseGraphicsPass {
    public: 
        using Ptr = std::shared_ptr<FullscreenPass>;

    private:
        BufferD3D12Impl::Ptr _quadVertexBuffer;
        BufferD3D12Impl::Ptr _quadIndexBuffer;
        uint _numRenderTarget;
    public:

        FullscreenPass(const std::wstring& psFileName, const std::string& passName, const std::wstring& entryPoint, ShaderModel sm, uint numRenderTarget) :
            ScenePass(L"BlitVS.hlsl", psFileName, passName, entryPoint, sm, ShaderType::Graphics, numRenderTarget)
        {
            _numRenderTarget = numRenderTarget;
            //FullscreenPass::Ptr fpp = std::make_shared<FullscreenPass>();

            /*fpp->_quadVertexBuffer = RenderEngine::CreateBuffer(4, sizeof(FSVertex));
            fpp->_quadIndexBuffer = RenderEngine::CreateBuffer(6, sizeof(uint));*/
            _quadVertexBuffer = Factory::CreateBuffer(4, sizeof(FSVertex));
            _quadIndexBuffer = Factory::CreateBuffer(6, sizeof(uint));


            FSVertex ver[4] = { {0.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f, 0.0f} };
            uint ind[6] = { 0, 1, 2, 0, 2, 3 };

            _quadVertexBuffer->SetBlob(ver, 4 * sizeof(FSVertex));
            _quadIndexBuffer->SetBlob(ind, 6 * sizeof(uint));


        }

        
        /*void BindCBVResource(matrix, 0) {

        }*/

        void Execute(RenderTargetD3D12Impl::Ptr rt) {
            Begin();

            RenderEngineD3D12Impl::Instance()->SetRenderTargets(_numRenderTarget, rt); 

            const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            RenderEngineD3D12Impl::Instance()->ClearRenderTargets(_numRenderTarget, rt, clearColor);
            
            RenderEngineD3D12Impl::Instance()->SetViewports();
            RenderEngineD3D12Impl::Instance()->SetScissorRects();
            

            RenderEngineD3D12Impl::Instance()->SetTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            RenderEngineD3D12Impl::Instance()->SetVertexBuffers(_quadVertexBuffer->GetVertexBufferView());
            RenderEngineD3D12Impl::Instance()->SetIndexBuffers(_quadIndexBuffer->GetIndexBufferView());
            RenderEngineD3D12Impl::Instance()->DrawInstanced(6, 1);

        }

        void End() {

        }
    };
}