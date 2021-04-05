
//namespace Global{
//    class RenderEngine{
//
//        std::map<std::string, Resource::Ptr> _rescMap;
//
//        Scene::Ptr _mpScene;    // 用来管理mesh等
//    };
//}
//
//class GraphicsPass{
//private:
//    std::map<std::string, int> _resc2Slot;
//public:
//    static void Create(const PassDesc& desc){
//
//    }
//    void BindResource(const string& resourceName, const uint resourceSlot);
//
//
//// TODO: 如何确定需要的状态是什么
//    void Execute(){
//
//        // 状态更新   场景 constant等
//
//
//        // 资源
//        for (auto& str : _resc2Slot){
//            _rescMap[str.first]
//        }
//
//
//        // 渲染
//    }
//};


//#include "../Core/AppFramework.h"
#include "simpleLTCApp.h"



int main() {
	AppFramework* app = new LTCApp(1280, 720);
	app->Run();

	return 0;
}



