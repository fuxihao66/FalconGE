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


		static Resource::Ptr CreateResource() {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<ResourceD3D12Impl>();
			default:
				return nullptr;
			}
		}

		static Buffer::Ptr CreateBuffer() {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<BufferD3D12Impl>();
			default:
				return nullptr;
			}
		}

		static Texture::Ptr CreateTexture(uint w, uint h) {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<TextureD3D12Impl>();
			default:
				return nullptr;
			}
		}
		static Texture::Ptr CreateTextureFromFile(const string& filename) {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<TextureD3D12Impl>(filename);
			default:
				return nullptr;
			}
		}

		static RenderTarget::Ptr CreateRenderTarget() {
			switch (Context::Instance().GetApi()) {
			case ApiType::D3D12:
				return make_shared<RenderTargetD3D12Impl>();
			default:
				return nullptr;
			}
		}
	};
}
