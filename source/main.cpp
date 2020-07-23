#include <iostream>

/*
 * 架构解释：
	 * app初始化前需要设置全局的gameContext config，使用Apply方法来应用（包括分辨率、api、窗口模式、延迟渲染等）
	 * 继承Scene，并在初始化方法中添加物体、材质和相机
	 * 这些资源会在这个Scene被设置为active的时候自动加载，inactive的时候自动销毁
	 * 加载后的资源会转化为api相关的表示，由RenderBackend实例来管理
	 * 渲染时RenderBackend实例直接将其中的所有资源渲染出来
	 * update方法目前在scene和appFramework中实现，切换场景可以在scene的update方法中通过SceneManager来切换
	 * 在app初始化中需要实例化所有的scene，并且添加到SceneManager中（并不会加载资源）
	 * 顶点动画可以通过在scene的update方法，动态地改变某个物体的几何，然后手动调用renderBackend的方法
 */

/*
 //TODO: 三个重要内容
 1. scene转dx12表示
 2. app 窗体  input
 3. 渲染，前端等
*/

/*
展示内容：分屏 分别是全局光照和局部光照 可以切换不同的全局光照
*/


#include "AppFramework.h"
#include "RenderBackend.h"
#include "Renderable.h"
#include "Scene.h"
#include "Utility.h"
#include "Config.h"



class StartupScene : public Falcon::Scene {
public:

	// 这一步加载资源，在renderBackend里面转化成api相关表示
	virtual void OnStart() { // 只有在setActive时才会被调用

		// 只需要添加物体 材质 和 配置摄像机
		// 底层的自动完成

		auto globalLightRenderEffect = LoadEffectFromFile("globalLight.xml", "globalLight");

		auto ball = LoadMeshFromFile("");
		// 使用geo指针和mat的key来构建
		auto renderable1 = std::make_shared<Falcon::Renderable>(ball, "globalLight", Falcon::Utility::IdentityMat());

		auto text = LoadTextureFromFile("/sdfsdf", "tex");

		// 材质参数绑定（纹理，浮点，vector）
		globalLightRenderEffect->SetPara("_mainTex", "tex");
		globalLightRenderEffect->SetPara("_lightPos", lightPos);
		globalLightRenderEffect->SetPara("_scale", scale);

		// render texture
		// 单独放到renderTexture列表中
		
		// TODO:  绑定纹理的时候，先搜索texArray，如果没有，搜索renderTexture（进行转换）
		auto rt1 = CreateRenderTexture(1920, 1080, xx, "renderTarget1");
		auto rt2 = CreateRenderTexture(1920, 1080, xx, "renderTarget2");

		
		// 相机（非main的先渲染）
		globalLightCam = std::make_shared<Camera>(Falcon::CamTag::RTCam, "globalLight", rt1);// 设置视口 scissor  只渲染一半
		locallLightCam = std::make_shared<Camera>(Falcon::CamTag::RTCam, "localLight", rt2);

		//mainCam = std::make_shared<Camera>(Falcon::CamTag::MainCam, sewingEffect, Falcon::RenderMode::OnlyAfterEffect);
		//SetMainCam(mainCam);
		AddCam(globalLightCam);
		AddCam(localLightCam);


		// 后处理效果（不会渲染场景，只会渲染一个四边形）
		auto sewingEffect = LoadEffectFromFile("sewingEffect.xml", "combineBlit");
		sewingEffect->SetTexture("LeftTexture", rt1);
		sewingEffect->SetTexture("RightTexture", rt2);

		AddAE(sewingEffect);
		


		Scene::OnStart();  // 这个调用保证所有资源被转化为底层表示
	}

	virtual void Update() {
		// TODO: 切换场景例子
		/*if (Falcon::InputManager::Instance().IsKeyDown()) {
			Falcon::SceneManager::Instance().SetActiveScene("test");
		}*/
	}
private:
	std::shared_ptr<Camera> globalLightCam;
	std::shared_ptr<Camera> localLightCam;
	std::shared_ptr<Camera> mainCam;

	
};


class TestApp : public Falcon::AppFramework {
public:
	TestApp() {

	}
protected:
	virtual void Initialize() {  

		AppFramework::Initialize();

		// 初始化各种参数  物体

		auto mainScene = std::make_shared<StartupScene>();


		// 把所有场景添加到sceneManager
		// 在每个scene里面 通过sceneManager的setActive来切换

		// add scene to active scene
		Falcon::SceneManager::Instance().AddScene(mainScene, "main");
		Falcon::SceneManager::Instance().SetActiveScene("main");  // setActiveScene 会进行所有物体的start 通过加载资源


		// 所有资源的准备工作需要在初始化完成，用以构建pipelineStateObject
	}

	// TODO: 目前在scene和app里可以添加update
	// TODO: 之后可以实现脚本系统来解决
	virtual void Update() {

	}
	// 
	
	//virtual void UpdateOverlay(); // left for UI object
};


int main() {

	// 第一阶段 最基本的框架  渲染一个三角形

	// 第二阶段 模型导入，纹理材质shader

	// 第三阶段 全局光照模型，分屏，基本输入控制


	Falcon::Config config = Falcon::Utility::LoadConfigFromFile("config.xml");

	// 设置渲染相关信息  创建渲染后端; 必须要在创建之前Apply
	Falcon::GameContext::Instance().Apply(config);

	TestApp app;

	app.Create();
	app.Run();

	return 0;
}