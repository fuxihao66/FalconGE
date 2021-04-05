#include "RenderPass.h"
#include "../Core/RenderTarget.h"
#include "../Core/Scene.h"
//#include "../GraphicsPass/FullscreenPass.h"

namespace Falcon
{
    class GbufferPass: public RenderPass{
    protected:
        Scene::Ptr mpScene;



        RenderTarget::Ptr rt;
        // shader path
        std::string gbShaderPath;
    public:
        virtual void SetScene(Scene::Ptr pScene) {
            mpScene = pScene;


        }
        virtual void Initialize(){


            gbpass = RenderEngine::Instance()->CreateGraphicsPass(gbShaderPath);


            Texture::Ptr Normal = Texture::Create();


            auto attachmens{Normal, WorldPos, MotionVec};

            rt->AddAttachments(attachments);     // 创建了连续的rtv





            //Buffer::Ptr matrix;
        }

        /*void UpdateMatrix(){
            auto camPos = mpScene->getCameraPosW();
            matrix->setBlob();
        }*/



        // 如何最大化并行
        // https://zhuanlan.zhihu.com/p/147207161
        virtual void Execute(){

            Begin();

            /*gbpass->Begin();
            gbpass->BindResource(matrix, "name of shader parameter");

            gbpass->SetRenderTarget(rt);
            gbpass->Execute();
            gbpass->End();*/

            mpScene->RenderScene(rt);




            End();
        }
    };
} // namespace Falcon

