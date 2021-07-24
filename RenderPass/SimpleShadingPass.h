#pragma once
#include <map>
#include "RenderPass.h"
#include "../GraphicsPass/FullscreenPass.h"


namespace Falcon{
    class SimpleShadingPass : public RenderPass{
    public:
        // input channels
        std::string kMotionVec = "";
        std::string kNormal = "";
        std::string kWorldPos = "";
        std::string kAlbedo = "";
        std::string kSpecular = "";
        std::string kRoughness = "";

        // output
        std::string kOutput = "SHADING_OUTPUT";

    private:
        RenderTargetD3D12Impl::Ptr renderTarget;

        ShaderResourceBindingD3D12Impl::Ptr srb;

        const std::string kShadingPassShader = "";

        Falcon::FullscreenPass::Ptr shadingPass;

        virtual void Initialize() {
            shadingPass = std::make_shared<Falcon::FullscreenPass>(kShadingPassShader, );

            auto w = RenderEngineD3D12Impl::Instance()->GetWidthHeight().x;
            auto h = RenderEngineD3D12Impl::Instance()->GetWidthHeight().y;

            auto tex = Factory::CreateTexture(w,h, kOutput);

            renderTarget = Factory::CreateRenderTarget();
            renderTarget->AddAttachments(vector<TextureD3D12Impl::Ptr>{tex});


            InitializeShaderResourceBinding();
        }


        virtual vector<ShaderObject::Ptr> GetShaderObjs() {
            return vector<ShaderObject::Ptr>{shadingPass->GetShaderObject()};

        }



        virtual void SetScene() {

        }

        void InitializeShaderResourceBinding() {

            if (kMotionVec == "") {
                // 错误
                return;
            }

            srb = shadingPass->GetShaderObject()->CreateShaderResourceBinding();
            auto bindingMap = std::map<string, ResourceD3D12Impl::Ptr>{
                { "gNormal", RenderEngineD3D12Impl::Instance()->GetResource(kNormal) },
                { "gMotionVec", RenderEngineD3D12Impl::Instance()->GetResource(kMotionVec) },
                { "gWorldPos", RenderEngineD3D12Impl::Instance()->GetResource(kWorldPos) },
                { "gAlbedo", RenderEngineD3D12Impl::Instance()->GetResource(kAlbedo) },
                { "gSpecular", RenderEngineD3D12Impl::Instance()->GetResource(kSpecular) },
                { "gRoughness", RenderEngineD3D12Impl::Instance()->GetResource(kRoughness) }};
            srb->BindResource(bindingMap);
        }

        void Execute(){
            RenderPassBegin();

            shadingPass->BindShaderResourceBinding(srb);

            shadingPass->Execute(renderTarget);
            RenderPassEnd();


        }
    };
}
