#include "Scene.h"
using namespace Falcon;

Scene::Scene() {
	_mainCam = nullptr;
}
// 加载Scene中资源；
void Scene::OnStart() {

	/*--------------------------资源转换-------------------------------*/
	//for (auto it = _matMap.begin(); it != _matMap.end(); it++) {
	//	// 绑定shader texture，参数
	//	RenderBackend::Instance().AddMat(it->second, it->first);

	//}
	RenderBackend::Instance().BuildMat(_matMap);
	for (auto it = _shaderMap.begin(); it != _shaderMap.end(); it++) {
		RenderBackend::Instance().BulidPSO(it->second, it->first);
	}
	// 纹理
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

	

	// 创建渲染的context，称为pass
	/*for (auto pcam : _camList) {
		RenderBackend::Instance().CreatePass();
	}*/

	//if (_mainCam != nullptr)
	//	RenderBackend::Instance().CreatePass(_mainCam);

	//for (auto pae : _aeContainer) {
	//	// 材质  参数  renderTarget
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
	
	// 切换前后缓冲
	RenderBackend::Instance().Flip();
}

void Scene::AddRenderable(std::shared_ptr<Renderable> obj) {
	_container.push_back(obj);
}

void Scene::OnUpdate() {
	// 位置更新

	/*for (auto it : _container) {
		it->OnUpdate();
	}*/


	// 物理引擎更新

	// 处理collision

	
}
void Scene::OnDestroy() {
	// 把纹理 mesh全部析构
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