#include "Scene.h"
using namespace Falcon;


// ����Scene����Դ��
void Scene::OnStart() {
	/*for (auto it : _container) {
		it->OnStart();
	}*/
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