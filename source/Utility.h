#pragma once

#include <string>

#include "tinyxml2/tinyxml2.h"

namespace tx = tinyxml2;

namespace Falcon {
	namespace Utility {
		// bunks of funcitons

		void LoadModelFromFile(const std::string& path) {

		}

		void LoadMaterialFromFile(const std::string& path) {

		}


		Config LoadConfigFromFile(const std::string& path) {
			tx::XMLDocument doc;
			doc.LoadFile(path.c_str());

			tx::XMLElement* root = doc.FirstChildElement("RenderConfig");

			tx::XMLElement* ResolutionElement		= root->FirstChildElement("Resolution");
			tx::XMLElement* RenderTypeElement		= root->FirstChildElement("RenderType");
			tx::XMLElement* MSAAElement				= root->FirstChildElement("MSAA");
			tx::XMLElement* ScreenModeElement		= root->FirstChildElement("ScreenMode");
			tx::XMLElement* RenderBackendElement	= root->FirstChildElement("RenderBackend");

			Config cfg;

			cfg.Resolution = Vec2<UINT>(atoi(ResolutionElement->FirstChildElement("x")->GetText()), atoi(ResolutionElement->FirstChildElement("y")->GetText()));
			if (strcmp(RenderTypeElement->GetText(), "Deferred") == 0)
				cfg.RType = RenderType::Deferred;

			if (strcmp(MSAAElement->GetText(), "Enabled") == 0)
				cfg.MSAA_Enabled = true;
			else
				cfg.MSAA_Enabled = false;

			if (strcmp(ScreenModeElement->GetText(), "FullScreen") == 0)
				cfg.SM = ScreenMode::FullScreen;
			else
				cfg.SM = ScreenMode::Windowed;

			if (strcmp(RenderBackendElement->GetText(), "DX12") == 0)
				cfg.API = ApiType::D3D12;
			else if (strcmp(RenderBackendElement->GetText(), "DX11") == 0)
				cfg.API = ApiType::D3D11;
			else if (strcmp(RenderBackendElement->GetText(), "OGL") == 0)
				cfg.API = ApiType::OGL;
			else if (strcmp(RenderBackendElement->GetText(), "Vulkan") == 0)
				cfg.API = ApiType::Vulkan;
			else
				cfg.API = ApiType::Nan;

			return cfg;
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

	enum RenderType {
		Deferred,
		ForwardPlus,
		Forward
	};

	using UINT = unsigned int;

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

	// TODO: 注意index的递增方向（先右还是先下）
	template <int row, int col, class T>
	struct Mat {
		std::array<T, row * col> _data;
		int RowNum = row;
		int ColNum = col;
	public:
		Mat() {
			for (size_t i = 0; i < row * col; i++) {
				_data[i] = static_cast<T>(0);
			}
		}
		T Get(int rIndex, int cIndex) const {
			return _data[rIndex * ColNum + cIndex];
		}
		//Mat<row, col, T> Multiply(const Mat<row, col, T>& another) const {
		//	// TODO: 保证列数等于行数
		//	Mat<row, col, T> result;
		//	for (size_t i = 0; i < result._data.size(); i++) {
		//		T temp = static_cast<T>(0);
		//		int _colIndex = ;
		//		int _rowIndex = i / ColNum;
		//		for (size_t j = 0; j < ColNum; j++) {
		//			temp += _data[_rowIndex * ColNum + j] * another._data[j * ColNum + _colIndex];
		//		}
		//		result._data[i] = temp;
		//	}
		//	return result;
		//}
		Mat<row, col, T> Add(const Mat<row, col, T>& another) const{
			Mat<row, col, T> result;
			for (size_t i = 0; i < result._data.size(); i++) {
				result = _data[i] + another._data[i];
			}
			return result;
		}
	};

	template <class T>
	using Mat4x4 = Mat<4, 4, T>;
}