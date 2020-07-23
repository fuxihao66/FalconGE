#include "Scene.h"
using namespace Falcon;


// 加载Scene中资源；
void Scene::OnStart() {
	/*for (auto it : _container) {
		it->OnStart();
	}*/

	// 纹理
	for (auto key : _texMap) {
		
		RenderBackend::Instance().AddTex(_texMap[key], key);
	}
	for (auto key : _shaderList) {
		RenderBackend::Instance().BulidPSO(_shaderList[key], key);
	}
	// 材质转换
	for (auto key : _matContainer) {
		// 绑定shader texture，参数
		RenderBackend::Instance().AddMat(_matContainer[key]); 
		
	}
	for (auto pObj : _objContainer) {
		// 
		RenderBackend::Instance().AddObj(pObj->Geo(), pObj->ModelMatrix, pObj->MatID);
	}

	// 创建渲染的context，称为pass
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

	// TODO: 这里不需要做任何事情，渲染直接用renderBackend来处理
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