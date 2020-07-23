#include "Scene.h"
using namespace Falcon;


// ����Scene����Դ��
void Scene::OnStart() {
	/*for (auto it : _container) {
		it->OnStart();
	}*/

	// ����
	for (auto key : _texMap) {
		
		RenderBackend::Instance().AddTex(_texMap[key], key);
	}
	for (auto key : _shaderList) {
		RenderBackend::Instance().BulidPSO(_shaderList[key], key);
	}
	// ����ת��
	for (auto key : _matContainer) {
		// ��shader texture������
		RenderBackend::Instance().AddMat(_matContainer[key]); 
		
	}
	for (auto pObj : _objContainer) {
		// 
		RenderBackend::Instance().AddObj(pObj->Geo(), pObj->ModelMatrix, pObj->MatID);
	}

	// ������Ⱦ��context����Ϊpass
	for (auto pcam : _camList) {
		RenderBackend::Instance().CreatePass();
		// add renderTexture
		RenderBackend::Instance().AddRenderTexture();
	}
	RenderBackend::Instance().CreatePass(_mainCam);

	for (auto pae : _aeContainer) {
		RenderBackend::Instance().AddAE();
	}

}

void Scene::OnRender() {  
	/*for (auto it : _container) {
		it->OnRender();
	}*/

	// TODO: ���ﲻ��Ҫ���κ����飬��Ⱦֱ����renderBackend������
}

void Scene::AddRenderable(std::shared_ptr<Renderable> obj) {
	_container.push_back(obj);
}

void Scene::OnUpdate() {
	// λ�ø���

	/*for (auto it : _container) {
		it->OnUpdate();
	}*/


	// �����������

	// ����collision

	
}
void Scene::OnDestroy() {
	// ������ meshȫ������
}


std::shared_ptr<MaterialBase> Scene::LoadEffectFromFile(const std::string& path, const std::string& key) {

	// parse xml
	auto shaderName = xxx;
	if (!_shaderList[shaderName])
		_shaderList[shaderName] = std::make_shared<Shader>(parsedShader);

	_matContainer[key] = std::make_shared<MaterialBase>(paraList, shaderName);

	return _matContainer[key];
}

std::shared_ptr<RenderTexture> Scene::CreateRenderTexture(int, int, , const std::string& id) {
	//_texMap[id] = std::shared_ptr<RenderTexture>();
}