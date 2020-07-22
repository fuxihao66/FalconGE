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
	template <class T>
	struct Vec3 {
		T x, y, z;
		Vec3<T>() {

		}
		Vec3<T>(const T& _x, const T& _y, const T& _z) {
			x = _x;
			y = _y;
			z = _z;
		}
	};

	template <class T>
	struct Vec4 {
		T x, y, z, w;
		using r = x;
		using g = y;
		using b = z;
		using a = w;
		using rgb = Vec3<T>(x, y, z);
		using xyz = Vec3<T>(x, y, z);
		Vec4<T>() {

		}
		Vec4<T>(const T& _x, const T& _y, const T& _z, const T& _w) {
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
	};

	template <int row, int col, class T>
	struct Mat {
		std::array<T, row * col> _data;
		int RowNum = row;
		int ColNum = col;
	public:
		T Get(int rIndex, int cIndex) {
			return _data[rIndex * ColNum + cIndex];
		}
		T Multiply(const Mat<row, col, T>& another) {

		}
		T Add(const Mat<row, col, T>& another) {

		}
	};

	template <class T>
	using Mat4x4 = Mat<4, 4, T>;
}