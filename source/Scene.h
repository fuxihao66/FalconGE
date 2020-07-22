#pragma once
#include <memory>

#include "Camera.h"
#include "Renderable.h"

namespace Falcon {
	class Scene
	{
	private:
		std::vector<std::shared_ptr<Renderable>> _container;
		std::vector<std::shared_ptr<Camera>> _camList;
		std::shared_ptr<Camera> _mainCam;
	public:
		//void Simulate();
		void OnStart();
		void OnRender();
		void OnUpdate();
		void OnDestroy();
		void AddRenderable(std::shared_ptr<Renderable> obj);
	};
}


