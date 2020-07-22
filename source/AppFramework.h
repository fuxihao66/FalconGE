#pragma once
#include <Windows.h>

#include "RenderBackend.h"
#include "Global.h"
namespace Falcon {
	class AppFramework
	{
	public:
		AppFramework();


		int Create();
		void Run();

	private:
		HWND hWnd;
		HINSTANCE hInstance;
	protected:
		float _physUpdateTime;
		float _simulationTime;   // time elapsed for physics

		//std::vector<std::shared_ptr<Renderable>> _objContainer;

		virtual void Initialize();
		virtual void Update();
		virtual void FixedUpdate(float timestep);
		virtual void Render();

	};

}

