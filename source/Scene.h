#pragma once
#include <memory>

#include "RenderBackend.h"
#include "Camera.h"
#include "Renderable.h"

namespace Falcon {
	class Scene
	{
	private:
		std::vector<std::shared_ptr<Renderable>> _objContainer;
		//std::vector<Mat4x4<float>> _modelMatrics;
		std::vector<std::shared_ptr<AfterEffect>> _aeContainer;
		std::vector<std::shared_ptr<Camera>> _camList;
		std::shared_ptr<Camera> _mainCam;
		std::vector<std::shared_ptr<RenderTarget>> _rtContainer;
		std::vector<std::shared_ptr<MaterialBace>> _matContainer;
		//std::vector<std::shared_ptr<Shader>> _shaderList;
	public:
		//void Simulate();
		virtual void OnStart();
		virtual void OnRender();
		virtual void OnUpdate();
		virtual void OnDestroy();
		void AddRenderable(std::shared_ptr<Renderable> obj);
		void AddCam();
		// ������������Ϊnullptr����renderBackend����Ϊ��������Ҫ����Ļ���
		void AddAE(std::shared_ptr<MaterialBase> afterEffect, std::shared_ptr<RenderTarget> output);

	};
}


