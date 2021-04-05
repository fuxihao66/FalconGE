#include <string>
#include <memory>
#include "BaseGraphicsPass.h"
#include "../Core/RenderEngine.h"

enum TOPOLOGY_TYPE{
    TRIANGLE = 0,
    TRIANGLE_STRIP = 1,
    POINT = 2
};
namespace Falcon {

    class FullscreenPass : public BaseGraphicsPass {
        
    using Ptr = std::shared_ptr<FullscreenPass>;

    private:
        Buffer::Ptr _quadVertexBuffer;
        Buffer::Ptr _quadIndexBuffer;

        ShaderObject::Ptr _fpShader;
    public:

        static FullscreenPass::Ptr Create(const std::string& path) {
            FullscreenPass::Ptr fpp = std::make_shared<FullscreenPass>();

            /*fpp->_quadVertexBuffer = RenderEngine::CreateBuffer(4, sizeof(FSVertex));
            fpp->_quadIndexBuffer = RenderEngine::CreateBuffer(6, sizeof(uint));*/
            fpp->_quadVertexBuffer = Buffer::Create(4, sizeof(FSVertex));
            fpp->_quadIndexBuffer = Buffer::Create(6, sizeof(uint));


            FSVertex ver[4] = { {0.0f, 1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 0.0f, 1.0f, 0.0f} };
            uint ind[6] = { 0, 1, 2, 0, 2, 3 };

            fpp->_quadVertexBuffer->setBlob(ver, 4 * sizeof(FSVertex));
            fpp->_quadIndexBuffer->setBlob(ind, 6 * sizeof(uint));


            _fpShader = ShaderObject::Create(path);
            //_fpShader = RenderEngine::CreateShaderObject(path);
        }

        void Begin() {
            //RenderEngine::SetPipeline(name);
            _fpShader->BindPipeline();
        }
        /*void BindCBVResource(matrix, 0) {

        }*/

        void Execute() {

            Falcon::RenderEngine::BindVertex();
            Falcon::RenderEngine::BindIndex();

            Falcon::RenderEngine::SetTopology(TOPOLOGY_TYPE::TRIANGLE);

            Falcon::RenderEngine::Draw();
        }

        void End() {

        }
    };
}