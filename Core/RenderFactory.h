#pragma once
/*
用于创建资源
*/
#include "../Utility/utility.h"
#include "Resource.h"
#include "RenderTarget.h"
#include "Context.h"
using std::make_shared;
using std::string;
namespace Falcon {
	class Factory {

	public:
		static ResourceD3D12Impl::Ptr CreateResource() {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<ResourceD3D12Impl>();
			default:
				return nullptr;
			}
		}

		static BufferD3D12Impl::Ptr CreateBuffer(uint numElements, uint elementSize) {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:

				return make_shared<BufferD3D12Impl>(numElements, elementSize);
			default:
				return nullptr;
			}
		}

		static TextureD3D12Impl::Ptr CreateTexture(uint w, uint h, const string& resourceName, DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT) {
			/*switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:*/
				TextureD3D12Impl::Ptr t = make_shared<TextureD3D12Impl>(w, h, format);
				RenderEngineD3D12Impl::Instance()->AddResourceToMap(resourceName, t);
				return t;
			//default:
				//return nullptr;
			//}
		}
		static TextureD3D12Impl::Ptr CreateTextureFromFile(const string& filename, const string& resourceName,DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT) {
			TextureD3D12Impl::Ptr t = make_shared<TextureD3D12Impl>(filename, format);
			RenderEngineD3D12Impl::Instance()->AddResourceToMap(resourceName, t);
			return t;
			/*switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<TextureD3D12Impl>(filename);
			default:
				return nullptr;
			}*/
		}

		static RenderTargetD3D12Impl::Ptr CreateRenderTarget() {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<RenderTargetD3D12Impl>();
			default:
				return nullptr;
			}
		}
	};
}
