#pragma once
#include "RenderPass.h"
#include "../Core/RenderTarget.h"
#include "../Core/Scene.h"
#include "../Core/RenderFactory.h"
//#include "../GraphicsPass/FullscreenPass.h"

namespace Falcon
{
    class GbufferPass: public RenderPass{
    protected:
        Scene::Ptr mpScene;



        RenderTargetD3D12Impl::Ptr rt;
        // shader path
        const std::string gbVSShaderPath = L"";
        const std::string gbPSShaderPath = L"";
    public:
        // output
        string kNormal = "GBUFFER_OUTPUT_NORMAL";
        string kMotionVec = "GBUFFER_OUTPUT_MoVec";
        string kAlbedo = "GBUFFER_OUTPUT_ALBEDO";
        string kSpecular = "GBUFFER_OUTPUT_SPECULAR";
        string kRoughness = "GBUFFER_OUTPUT_ROUGHNESS";
        string kWorldPos = "GBUFFER_OUTPUT_WorldPos";
    


        virtual void SetScene(Scene::Ptr pScene) {
            mpScene = pScene;

            mpScene->CreatePassFromFile(gbVSShaderPath, gbPSShaderPath, "gbufferPass", L"main", ShaderModel::SHADER_MODEL_6_0, ShaderType::Graphics, 6);
        }

        virtual vector<ShaderObject::Ptr> GetShaderObjs() {
            return vector<ShaderObject::Ptr>{mpScene->GetShaderObject()};
        }

        virtual void Initialize(){
            rt = Factory::CreateRenderTarget();


            //gbpass = RenderEngine::Instance()->CreateGraphicsPass(gbShaderPath);
            mpScene->CreatePassFromFile(gbShaderPath);

            auto w = RenderEngineD3D12Impl::Instance()->GetWidthHeight().x;
            auto h = RenderEngineD3D12Impl::Instance()->GetWidthHeight().y;

            auto Normal = Factory::CreateTexture(w, h, kNormal);
            auto MotionVec = Factory::CreateTexture(w, h, kMotionVec);
            auto Albedo = Factory::CreateTexture(w, h, kAlbedo);
            auto Specular = Factory::CreateTexture(w, h, kSpecular);
            auto Roughness = Factory::CreateTexture(w, h, kRoughness);
            auto WorldPos = Factory::CreateTexture(w, h, kWorldPos);


            vector<TextureD3D12Impl::Ptr> attachments{Normal, WorldPos, Albedo, Specular, Roughness, MotionVec};

            rt->AddAttachments(attachments);     // 创建了连续的rtv





            //Buffer::Ptr matrix;
        }


        // 如何最大化并行
        // https://zhuanlan.zhihu.com/p/147207161
        virtual void Execute(){

            RenderPassBegin();
            mpScene->RenderScene(6, rt);

            RenderPassEnd();
        }
    };
} // namespace Falcon

