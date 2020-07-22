#include "Scene.h"
using namespace Falcon;


// ����Scene����Դ��
void Scene::OnStart() {
	/*for (auto it : _container) {
		it->OnStart();
	}*/

	// ����
	for (auto pMat : _matContainer) {
		// ͨ��renderBackend�Ľӿڽ���ת��
		RenderBackend::Instance().AddTex(pMat);
	}
	// ����ת��
	for (auto pMat : _matContainer) {
		// ͨ��renderBackend�Ľӿڽ���ת��
		RenderBackend::Instance().AddMat(pMat); // ͨ��map������tex
	}
	for (auto pObj : _objContainer) {
		// 
		RenderBackend::Instance().AddObj(pObj->Geo(), pObj->ModelMatrix, pObj->MatIndex);
	}

	// ������Ⱦ��context����Ϊpass
	for (auto pcam : _camList) {
		RenderBackend::Instance().CreatePass();
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