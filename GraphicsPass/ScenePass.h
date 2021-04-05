#include <string>

#include "BaseGraphicsPass.h"
#include "../Core/RenderEngine.h"

enum TOPOLOGY_TYPE{
    TRIANGLE = 0,
    TRIANGLE_STRIP = 1,
    POINT = 2
};

namespace Falcon {

    class ScenePass : public BaseGraphicsPass {
        // vertex buffer


        static ScenePass::Ptr Create(const std::string& path) {



        }

        void Begin() {
            Falcon::RenderEngine::Instance()->BindInputSetPipeline(name);
        }
        void BindCBVResource(matrix, 0) {

        }

        void Execute() {

            Falcon::RenderEngine::Instance()->BindInputBindVertex();
            Falcon::RenderEngine::Instance()->BindInputBindIndex();

            Falcon::RenderEngine::Instance()->BindInputSetTopology(TOPOLOGY_TYPE::TRIANGLE);

            Falcon::RenderEngine::Instance()->BindInputDraw();


            //CD3DX12_CPU_DESCRIPTOR_HANDLE intermediateRtv(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), renderTargetPtr->GetRTVDescriptor(), m_rtvDescriptorSize);
            //m_commandList->OMSetRenderTargets(1, &intermediateRtv, FALSE, nullptr);

            //// const float clearColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
            //m_commandList->ClearRenderTargetView(intermediateRtv, renderTargetPtr->clearColor(), 0, nullptr);


            //m_commandList->IASetVertexBuffers(0, 1, &m_gradientVertexBufferView);
            //m_commandList->DrawInstanced(4, 1, 0, 0);
        }
    };
}