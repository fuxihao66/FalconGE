#pragma once

#include <string>

namespace Falcon {
	namespace Utility {
		// bunks of funcitons

		void LoadModelFromFile(const std::string& path) {

		}

		void LoadMaterialFromFile(const std::string& path) {

		}


		Config LoadConfigFromFile(const std::string& path) {

		}
	}

	enum CamTag {
		MainCam = 0,
		RTCam = 1,
		Disabled = 2
	};

	enum ApiType {
		D3D12 = 0,
		D3D11 = 1,
		OGL = 2,
		Vulkan = 3,
		Nan = 4
	};

	enum ScreenMode {
		FullScreen = 0,
		Windowed = 1
	};


	template <class T>
	struct Vec2 {
		T x, y;
		Vec2<T>() {

		}
		Vec2<T>(const T& _x, const T& _y) {
			x = _x;
			y = _y;
		}
	};




}