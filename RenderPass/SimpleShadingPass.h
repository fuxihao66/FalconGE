#include "RenderPass.h"
#include "../GraphicsPass/FullscreenPass.h"


namespace Falcon{
    class SimpleShadingPass : public RenderPass{
    public:
        // input channels
        std::string kMotionVec;

    private:

        const std::string kShadingPassShader = "";


        



        Falcon::FullscreenPass::SharedPtr shadingPass;


        // std::string GetResource(){

        // }

        void InitializeShader(){
            shadingPass = Falcon::FullscreenPass::create(kShadingPassShader);


        }


        virtual void SetScene() {

        }

        void InitializeShaderResourceBinding() {

            if (kMotionVec == "") {
                // 错误
            }

            auto srb = shadingPass->CreateShaderResourceBinding();
        }

        void UpdateData(){

        }

        void Execute(){
            shadingPass->Begin();
            shadingPass.BindSRVResource(kMotionVec, 0);   // slot对应register
            shadingPass.BindSRVResource(, 1);   

            shadingPass->Execute();
            shadingPass->End();

        }
    };
}
