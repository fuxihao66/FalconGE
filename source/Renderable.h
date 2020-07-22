#pragma once
#include <memory>
#include <vector>
#include <functional>

#include "Error.h"

namespace Falcon {
	class Renderable
	{
	private:
		std::shared_ptr<Mesh> _geo;
		std::shared_ptr<Material> _mat;
		std::shared_ptr<PhysicsProp> _phys;

		/*std::vector<std::function<void()>> CollisionCallback;
		std::vector<std::function<void()>> TriggerCallback;
		std::vector<std::function<void()>> UpdateCallback;
		std::vector<std::function<void()>> StartCallback;*/

	public:
		/*Falcon::ErrorType AddCallBackUpdate(std::function<void()>& f);
		Falcon::ErrorType AddCallBackStart(std::function<void()>& f);
		Falcon::ErrorType AddCallBackTrigger(std::function<void()>& f);
		Falcon::ErrorType AddCallBackCollision(std::function<void()>& f);*/

		//Falcon::ErrorType Render();

		virtual void OnDestroy();
		virtual void OnRender();
		virtual void OnUpdate();
		virtual void OnStart();    // 在scene OnStart 中被调用 加载资源

		virtual void OnTriggerEnter();
		virtual void OnTriggerLeave();
		virtual void OnCollisionEnter();
		virtual void OnCollisionLeave();
	};
}


