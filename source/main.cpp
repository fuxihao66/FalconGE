#include <iostream>

/*
 * �ܹ����ͣ�
	 * app��ʼ��ǰ��Ҫ����ȫ�ֵ�gameContext config��ʹ��Apply������Ӧ�ã������ֱ��ʡ�api������ģʽ���ӳ���Ⱦ�ȣ�
	 * �̳�Scene�����ڳ�ʼ��������������塢���ʺ����
	 * ��Щ��Դ�������Scene������Ϊactive��ʱ���Զ����أ�inactive��ʱ���Զ�����
	 * ���غ����Դ��ת��Ϊapi��صı�ʾ����RenderBackendʵ��������
	 * ��ȾʱRenderBackendʵ��ֱ�ӽ����е�������Դ��Ⱦ����
	 * update����Ŀǰ��scene��appFramework��ʵ�֣��л�����������scene��update������ͨ��SceneManager���л�
	 * ��app��ʼ������Ҫʵ�������е�scene��������ӵ�SceneManager�У������������Դ��
	 * ���㶯������ͨ����scene��update��������̬�ظı�ĳ������ļ��Σ�Ȼ���ֶ�����renderBackend�ķ���
 */

/*
 //TODO: ������Ҫ����
 1. sceneתdx12��ʾ
 2. app ����  input
 3. ��Ⱦ��ǰ�˵�
*/

/*
չʾ���ݣ����� �ֱ���ȫ�ֹ��պ;ֲ����� �����л���ͬ��ȫ�ֹ���
*/


#include "AppFramework.h"
#include "RenderBackend.h"
#include "Renderable.h"
#include "Scene.h"
#include "Utility.h"
#include "Config.h"



class StartupScene : public Falcon::Scene {
public:

	// ��һ��������Դ����renderBackend����ת����api��ر�ʾ
	virtual void OnStart() { // ֻ����setActiveʱ�Żᱻ����

		// ֻ��Ҫ������� ���� �� ���������
		// �ײ���Զ����

		auto globalLightRenderEffect = LoadEffectFromFile("globalLight.xml", "globalLight");

		auto ball = LoadMeshFromFile("");
		// ʹ��geoָ���mat��key������
		auto renderable1 = std::make_shared<Falcon::Renderable>(ball, "globalLight", Falcon::Utility::IdentityMat());

		auto text = LoadTextureFromFile("/sdfsdf", "tex");

		// ���ʲ����󶨣��������㣬vector��
		globalLightRenderEffect->SetPara("_mainTex", "tex");
		globalLightRenderEffect->SetPara("_lightPos", lightPos);
		globalLightRenderEffect->SetPara("_scale", scale);

		// render texture
		// �����ŵ�renderTexture�б���
		
		// TODO:  �������ʱ��������texArray�����û�У�����renderTexture������ת����
		auto rt1 = CreateRenderTexture(1920, 1080, xx, "renderTarget1");
		auto rt2 = CreateRenderTexture(1920, 1080, xx, "renderTarget2");

		
		// �������main������Ⱦ��
		globalLightCam = std::make_shared<Camera>(Falcon::CamTag::RTCam, "globalLight", rt1);// �����ӿ� scissor  ֻ��Ⱦһ��
		locallLightCam = std::make_shared<Camera>(Falcon::CamTag::RTCam, "localLight", rt2);

		//mainCam = std::make_shared<Camera>(Falcon::CamTag::MainCam, sewingEffect, Falcon::RenderMode::OnlyAfterEffect);
		//SetMainCam(mainCam);
		AddCam(globalLightCam);
		AddCam(localLightCam);


		// ����Ч����������Ⱦ������ֻ����Ⱦһ���ı��Σ�
		auto sewingEffect = LoadEffectFromFile("sewingEffect.xml", "combineBlit");
		sewingEffect->SetTexture("LeftTexture", rt1);
		sewingEffect->SetTexture("RightTexture", rt2);

		AddAE(sewingEffect);
		


		Scene::OnStart();  // ������ñ�֤������Դ��ת��Ϊ�ײ��ʾ
	}

	virtual void Update() {
		// TODO: �л���������
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

		// ��ʼ�����ֲ���  ����

		auto mainScene = std::make_shared<StartupScene>();


		// �����г�����ӵ�sceneManager
		// ��ÿ��scene���� ͨ��sceneManager��setActive���л�

		// add scene to active scene
		Falcon::SceneManager::Instance().AddScene(mainScene, "main");
		Falcon::SceneManager::Instance().SetActiveScene("main");  // setActiveScene ��������������start ͨ��������Դ


		// ������Դ��׼��������Ҫ�ڳ�ʼ����ɣ����Թ���pipelineStateObject
	}

	// TODO: Ŀǰ��scene��app��������update
	// TODO: ֮�����ʵ�ֽű�ϵͳ�����
	virtual void Update() {

	}
	// 
	
	//virtual void UpdateOverlay(); // left for UI object
};


int main() {

	// ��һ�׶� ������Ŀ��  ��Ⱦһ��������

	// �ڶ��׶� ģ�͵��룬�������shader

	// �����׶� ȫ�ֹ���ģ�ͣ������������������


	Falcon::Config config = Falcon::Utility::LoadConfigFromFile("config.xml");

	// ������Ⱦ�����Ϣ  ������Ⱦ���; ����Ҫ�ڴ���֮ǰApply
	Falcon::GameContext::Instance().Apply(config);

	TestApp app;

	app.Create();
	app.Run();

	return 0;
}