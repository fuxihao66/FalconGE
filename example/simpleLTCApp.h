#include <map>
#include "../Core/RenderEngine.h"
#include "../Core/AppFramework.h"
#include "../RenderPass/GbufferPass.h"
#include "../RenderPass/SimpleShadingPass.h"
// 实现falcor中脚本的功能
class LTCApp: public AppFramework{
    std::map<std::string, Falcon::RenderPass::Ptr> _passMap;

public:
    LTCApp() {}
    LTCApp(int w, int h);
    virtual void Initialize();

    virtual void Execute();

    
};
LTCApp::LTCApp(int w, int h) : AppFramework(w,h)
{

}

void LTCApp::Initialize() {

    auto pScene = Falcon::Utility::LoadScene("xx.fscene");


    _passMap["gbufferPass"] = Falcon::GBufferPass::Create();
    // _passMap["ltcPass"] = Falcon::LTCPass::Create();   // 这个pass包括所有东西,ltc shadow
    // _passMap["svgfPass"] = Falcon::SVGFPass::Create();


    _passMap["shadingPass"] = Falcon::SimpleShadingPass::Create();


    _passMap["gbufferPass"]->SetScene(pScene);

    Falcon::RenderEngine::Instance()->BindInput(_passMap["shadingPass"].kMotionVec, _passMap["gbufferPass"].kMotionVec);   // output input   map<string, string> 当前pass变量名称<-> 全局的resource map的key 
    Falcon::RenderEngine::Instance()->BindInput(_passMap["shadingPass"].kNormal, _passMap["gbufferPass"].kNormal);
    Falcon::RenderEngine::Instance()->BindInput(_passMap["shadingPass"].kWorldPos, _passMap["gbufferPass"].kWorldPos);
    Falcon::RenderEngine::Instance()->BindInput(_passMap["shadingPass"].kAlbedo, _passMap["gbufferPass"].kAlbedo);
    Falcon::RenderEngine::Instance()->BindInput(_passMap["shadingPass"].kSpecular, _passMap["gbufferPass"].kSpecular);
    Falcon::RenderEngine::Instance()->BindInput(_passMap["shadingPass"].kRoughness, _passMap["gbufferPass"].kRoughness);

    Falcon::RenderEngine::Instance()->AddOutput(_passMap["shadingPass"].kOutput);


    for (auto pass : _passMap) {
        pass.second->Initialize();
    }
}
void LTCApp::Execute() {

    _passMap["gbufferPass"]->execute();


    // TODO: rendergraph中  入度为0的开始执行，按照拓扑排序执行  或者要求开发者自行决定
    // barrier??  
    _passMap["shadingPass"]->execute();

}