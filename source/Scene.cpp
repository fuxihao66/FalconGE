#include "Scene.h"
using namespace Falcon;

Scene::Scene() {
	_mainCam = nullptr;
}
// ����Scene����Դ��
void Scene::OnStart() {

	/*--------------------------��Դת��-------------------------------*/
	//for (auto it = _matMap.begin(); it != _matMap.end(); it++) {
	//	// ��shader texture������
	//	RenderBackend::Instance().AddMat(it->second, it->first);

	//}
	RenderBackend::Instance().BuildMat(_matMap);
	for (auto it = _shaderMap.begin(); it != _shaderMap.end(); it++) {
		RenderBackend::Instance().BulidPSO(it->second, it->first);
	}
	// ����
	for (auto it = _texMap.begin(); it != _texMap.end(); it++ ) {
		
		RenderBackend::Instance().AddTex(it->second, it->first);
	}

	for (auto it = _rtMap.begin(); it != _rtMap.end(); it++) {
		RenderBackend::Instance().AddRenderTexture(it->second, it->first);
	}
	
	for (auto it = _objMap.begin(); it != _objMap.end(); it++ ) {
		// 
		RenderBackend::Instance().AddObj(it->second, it->first);
	}

	

	// ������Ⱦ��context����Ϊpass
	/*for (auto pcam : _camList) {
		RenderBackend::Instance().CreatePass();
	}*/

	//if (_mainCam != nullptr)
	//	RenderBackend::Instance().CreatePass(_mainCam);

	//for (auto pae : _aeContainer) {
	//	// ����  ����  renderTarget
	//	RenderBackend::Instance().AddAE();
	//}

}

void Scene::OnRender() {  
	
	for (auto pcam : _camList)
		RenderBackend::Instance().DoRender(pcam);
	if (_mainCam != nullptr)
		RenderBackend::Instance().DoRender(_mainCam);
	for (auto pae : _aeContainer) {
		RenderBackend::Instance().DoAE(pae);
	}
	
	// �л�ǰ�󻺳�
	RenderBackend::Instance().Flip();
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
	_objMap.clear();
	_aeContainer.clear();
	_rtMap.clear();
	_shaderMap.clear();
	_texMap.clear();
	_matMap.clear();
	_camList.clear();
	_mainCam = nullptr;

}


std::shared_ptr<MaterialBase> Scene::LoadEffectFromFile(const std::string& path, const std::string& key) {

	tx::XMLDocument doc;
	doc.LoadFile(path.c_str());

	tx::XMLElement* root = doc.FirstChildElement("MatInfo");

	tx::XMLElement* ParaElement = root->FirstChildElement("Parameter");
	tx::XMLElement* ColorElement = root->FirstChildElement("Color");
	tx::XMLElement* TextureElement = root->FirstChildElement("Texture");
	tx::XMLElement* FloatElement = root->FirstChildElement("Float");
	auto paraList;

	tx::XMLElement* ShaderElement = root->FirstChildElement("Shader");


	// parse xml
	std::string shaderName = ShaderElement->FirstChildElement("ShaderName")->GetText();
	if (!_shaderMap[shaderName])
		_shaderMap[shaderName] = std::make_shared<Shader>(ShaderElement->FirstChildElement("ShaderSource")->GetText());

	_matMap[key] = std::make_shared<MaterialBase>(paraList, shaderName);

	return _matMap[key];
}

std::shared_ptr<RenderTexture> Scene::CreateRenderTexture(int, int, , const std::string& id) {
	//_texMap[id] = std::shared_ptr<RenderTexture>();
}