#pragma once
#include <map>
#include "../Core/RenderEngine.h"
#include "../Core/AppFramework.h"
#include "../RenderPass/GbufferPass.h"
#include "../RenderPass/SimpleShadingPass.h"
// 实现falcor中脚本的功能
class LTCApp: public AppFramework{

public:
    LTCApp() {}
    LTCApp(int w, int h);
    virtual void Initialize();

    virtual void ExecuteImpl();

    
};
LTCApp::LTCApp(int w, int h) : AppFramework(w,h)
{

}

void LTCApp::Initialize() {

    auto pScene = Falcon::LoadScene("xx.fscene");


    
    auto gbufferPass = std::make_shared<Falcon::GbufferPass>();
    // _passMap["ltcPass"] = Falcon::LTCPass::Create();   // 这个pass包括所有东西,ltc shadow
    // _passMap["svgfPass"] = Falcon::SVGFPass::Create();
    auto shadingPass = std::make_shared<Falcon::SimpleShadingPass>();

     
    gbufferPass->SetScene(pScene);

    Falcon::RenderEngineD3D12Impl::Instance()->BindInput(shadingPass->kMotionVec, gbufferPass->kMotionVec);   // output input   map<string, string> 当前pass变量名称<-> 全局的resource map的key 
    Falcon::RenderEngineD3D12Impl::Instance()->BindInput(shadingPass->kNormal, gbufferPass->kNormal);
    Falcon::RenderEngineD3D12Impl::Instance()->BindInput(shadingPass->kWorldPos, gbufferPass->kWorldPos);
    Falcon::RenderEngineD3D12Impl::Instance()->BindInput(shadingPass->kAlbedo, gbufferPass->kAlbedo);
    Falcon::RenderEngineD3D12Impl::Instance()->BindInput(shadingPass->kSpecular, gbufferPass->kSpecular);
    Falcon::RenderEngineD3D12Impl::Instance()->BindInput(shadingPass->kRoughness, gbufferPass->kRoughness);

    Falcon::RenderEngineD3D12Impl::Instance()->AddOutput(shadingPass->kOutput);

    _passMap["gbufferPass"] = gbufferPass;
    _passMap["shadingPass"] = shadingPass;
   
    AppFramework::Initialize();
}
void LTCApp::ExecuteImpl() {
    _passMap["gbufferPass"]->Execute();


    // TODO: rendergraph中  入度为0的开始执行，按照拓扑排序执行  或者要求开发者自行决定
    // barrier??  
    _passMap["shadingPass"]->Execute();


}