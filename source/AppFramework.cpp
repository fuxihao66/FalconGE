#include "AppFramework.h"

using namespace Falcon;

AppFramework::AppFramework() {
	_selfPointer = this;
}

AppFramework& AppFramework::Instance() {
	return *_selfPointer;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return AppFramework::Instance().MsgProc(hWnd, uMsg, wParam, lParam);
}

LRESULT AppFramework::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			_appPaused = true;
			Timer::Instance().Pause();
		}
		else
		{
			_appPaused = false;
			Timer::Instance().Start();
		}
		return 0;
		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		_windowWidth = LOWORD(lParam);
		_windowHeight = HIWORD(lParam);

		if (wParam == SIZE_MINIMIZED)
		{
			_appPaused = true;
			_minimized = true;
			_maximized = false;
		}
		else if (wParam == SIZE_MAXIMIZED)
		{
			_appPaused = false;
			_minimized = false;
			_maximized = true;
			OnResize();
		}
		else if (wParam == SIZE_RESTORED)
		{

			// Restoring from minimized state?
			if (_minimized)
			{
				_appPaused = false;
				_minimized = false;
				OnResize();
			}

			// Restoring from maximized state?
			else if (_maximized)
			{
				_appPaused = false;
				_maximized = false;
				OnResize();
			}
			else if (_resizing)
			{
				// If user is dragging the resize bars, we do not resize 
				// the buffers here because as the user continuously 
				// drags the resize bars, a stream of WM_SIZE messages are
				// sent to the window, and it would be pointless (and slow)
				// to resize for each WM_SIZE message received from dragging
				// the resize bars.  So instead, we reset after the user is 
				// done resizing the window and releases the resize bars, which 
				// sends a WM_EXITSIZEMOVE message.
			}
			else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
			{
				OnResize();
			}

		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		_appPaused = true;
		_resizing = true;
		Timer::Instance().Pause();
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		_appPaused = false;
		_resizing = false;
		Timer::Instance().Start();
		OnResize();
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;


		/*operate input*/
	case WM_LBUTTONDOWN:
		InputManager::Instance().SetMouseDown(Mouse::Left);
		InputManager::Instance().SetMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MBUTTONDOWN:
		InputManager::Instance().SetMouseDown(Mouse::Middle);
		InputManager::Instance().SetMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONDOWN:
		InputManager::Instance().SetMouseDown(Mouse::Right);
		InputManager::Instance().SetMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
		InputManager::Instance().SetMouseUp(Mouse::Left);
		InputManager::Instance().SetMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MBUTTONUP:
		InputManager::Instance().SetMouseUp(Mouse::Middle);
		InputManager::Instance().SetMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
		InputManager::Instance().SetMouseUp(Mouse::Right);
		InputManager::Instance().SetMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		InputManager::Instance().SetMousePos(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;

	case WM_MOUSEWHEEL:
		// return GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1 : -1;
		return 0;

	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		InputManager::Instance().SetKeyUp(static_cast<long>(wParam));

		return 0;
	case WM_KEYDOWN:
		
		InputManager::Instance().SetKeyDown(static_cast<long>(wParam));
	}


	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int AppFramework::Create(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow) {

    Initialize();

    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Falcon";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

	hWnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Falcon",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );
	
    if (hWnd == NULL)
    {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
}

void AppFramework::OnUpdate() {
    // 
    SceneManager::Instance().OnUpdate();
}

//
//void AppFramework::FixedUpdate(float timestep) {
//        it->Simulate();
//}

int AppFramework::Run() {  // 开始循环

    Timer::Instance().Reset();

	// TODO: 用于精确物理模拟
	//while (1) {   // 获取窗体推出 信号
 //       double realTime = Timer::GetTime();  // timer 全局实例

 //       // get message
 //       // dispatch message

 //       Update();

 //       while (_simulationTime < realTime) {
 //           _simulationTime += _physUpdateTime;
 //           FixedUpdate(_physUpdateTime);
 //       }

 //       Render();
	//}

	bool gotMsg;
	MSG  msg;

	::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE);

	
	while (WM_QUIT != msg.message)
	{
		/*if (main_wnd_->Active())
		{
			gotMsg = (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0);
		}
		else
		{
			gotMsg = (::GetMessage(&msg, nullptr, 0, 0) != 0);
		}*/

		if (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			// input manager
		}
		else
		{
			Timer::Instance().Tick();
			if (!_appPaused) {
				OnUpdate();
				OnRender();
			}
			else {
				Sleep(100);
			}
		}

	}
	return (int)msg.wParam;

}

void AppFramework::OnRender() {
	//RenderBackend::Instance().Render();
}