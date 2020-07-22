#pragma once
#include <memory>
#include <map>
#include <string>

#include "Scene.h"
#include "Utility.h"
#include "Config.h"
// 全局实例

namespace Falcon {
	class InputManager {
	private:
		static std::shared_ptr<InputManager> _self;
		std::map<long, bool> _keyMapping;
		bool _mouseLeftDown;
		bool _mouseRightDown;
		bool _mouseMiddleDown;
		float _cursorX;
		float _cursorY;
		InputManager() {

		}

	public:
		static InputManager& Instance() {
			if (_self == nullptr) {
				_self = std::make_shared<InputManager>();
			}
			return *_self;
		}

		void SetMouseDown(UINT para) {
			switch (para) {
			case Mouse::Left:
				_mouseLeftDown = true;
				break;
			case Mouse::Right:
				_mouseRightDown = true;
				break;
			case Mouse::Middle:
				_mouseMiddleDown = true;
			}
		}
		void SetMousePos(float x, float y) {
			_cursorX = x;
			_cursorY = y;
		}

		void SetMouseUp(UINT para) {
			switch (para) {
			case Mouse::Left:
				_mouseLeftDown = false;
				break;
			case Mouse::Right:
				_mouseRightDown = false;
				break;
			case Mouse::Middle:
				_mouseMiddleDown = false;
			}
		}
		void SetKeyDown() {

		}

		void SetKeyUp() {

		}

		bool IsKeyDown(long key) {
			return _keyMapping[key];
		}
		bool IsKeyUp(long key) {
			return !_keyMapping[key];
		}

		bool IsMouseLeftDown() {
			return _mouseLeftDown;
		}
		bool IsMouseLeftUp() {
			return !_mouseLeftDown;
		}
		bool IsMouseRightDown() {
			return _mouseRightDown;
		}
		bool IsMouseRightUp() {
			return !_mouseRightDown;
		}
		bool IsMouseMiddleDown() {
			return _mouseMiddleDown;
		}
		bool IsMouseMiddleUp() {
			return !_mouseMiddleDown;
		}
	};


	class SceneManager {
	private:
		static std::unique_ptr<SceneManager> _selfPointer;
		std::map<std::string, std::shared_ptr<Scene>> _sceneMapper;
		std::shared_ptr<Scene> _currentScene;
	public:
		static SceneManager& Instance() {
			if (_selfPointer == nullptr) {
				_selfPointer = std::make_unique<SceneManager>();
			}
			return *_selfPointer;
		}
		void OnUpdate() {
			_currentScene->OnUpdate();
		}
		bool SetActiveScene(const std::string& name) {
			if (!_sceneMapper[name]) {
				// error 
				return false;
			}

			if (_currentScene)
				_currentScene->OnDestroy();
			_currentScene = _sceneMapper[name];
			_currentScene->OnStart();
			return true;
		}
		void AddScene(std::shared_ptr<Scene> s, const std::string& name) {
			_sceneMapper[name] = s;
		}
	};

	class Timer {
	private:
		double _lastStart;
		double _lastPause;
	public:
		
		static void Start() {
			//_lastStart = ;
		}

	};

	// 
	class GameContext {
		Vec2<UINT>	Resolution;
		bool		MSAA_Enabled;
		ScreenMode	SM;
		ApiType		API;

		bool HasApplied;
		//std::unique_ptr<RenderBackend> renderer;
	private:
		static std::unique_ptr<GameContext> pSelf;
		GameContext() {
			// 初始设定
			Resolution = Vec2<UINT>(1920, 1080);
			MSAA_Enabled = false;
			SM = ScreenMode::FullScreen;
			API = ApiType::D3D12;
		}
	public:
		static GameContext& Instance() {
			if (pSelf == nullptr) {
				pSelf = std::make_unique<GameContext>();
			}
			return *pSelf;
		}
		void Apply(Config cfg) {
			pSelf->Resolution = cfg.Resolution;
			pSelf->MSAA_Enabled = cfg.MSAA_Enabled;
			pSelf->SM = cfg.SM;
			pSelf->API = cfg.API;

		}
		ApiType GetApi() {
			return API;
		}
	};

}