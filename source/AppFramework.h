#pragma once
#include <Windows.h>
#include <windowsx.h>
#include "RenderBackend.h"
#include "Global.h"
namespace Falcon {
	class AppFramework
	{
	public:
		AppFramework();

		

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		int Create(HINSTANCE hInstance, HINSTANCE hPrevInstance,
			PSTR lpCmdLine, INT nCmdShow);
		int Run();

		static AppFramework& Instance();
	private:
		static AppFramework* _selfPointer;
		HWND hWnd;
		HINSTANCE hInstance;
	protected:
		float _physUpdateTime;
		float _simulationTime;   // time elapsed for physics

		bool _appPaused;
		bool _resizing;
		bool _minimized;
		bool _maximized;
		UINT _windowWidth;
		UINT _windowHeight;

		//std::vector<std::shared_ptr<Renderable>> _objContainer;

		virtual void Initialize();

		void OnResize();
		void OnUpdate();
		//virtual void FixedUpdate(float timestep);
		void OnRender();

	};

}

