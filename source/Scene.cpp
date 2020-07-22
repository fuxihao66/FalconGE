#include "Scene.h"
using namespace Falcon;


// 加载Scene中资源；
void Scene::OnStart() {
	/*for (auto it : _container) {
		it->OnStart();
	}*/
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