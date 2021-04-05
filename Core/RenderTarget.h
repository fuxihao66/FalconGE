#include "../Utility/utility.h"

#include "RenderEngine.h"
#include "Resource.h"
using std::shared_ptr;


namespace Falcon {
	class RenderTarget {

	public:
		using Ptr = shared_ptr<RenderTarget>;
		virtual void AddAttachments(Texture::Ptr* buffers, uint numAttachments);
		virtual void AddAttachments(Texture::Ptr* buffers, uint numAttachments, Texture::Ptr depthStencilBuffer);
	};


	class RenderTargetD3D12Impl : public RenderTarget {
	private:
		int _numAttachments;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	public:
		using Ptr = shared_ptr<RenderTargetD3D12Impl>;

		virtual void AddAttachments(Texture::Ptr* buffers, uint numAttachments);
		virtual void AddAttachments(Texture::Ptr* buffers, uint numAttachments, Texture::Ptr depthStencilBuffer);

	};

	void RenderTargetD3D12Impl::AddAttachments(Texture::Ptr* buffers, uint numAttachments) {
		// 创建rtv  绑定到这些texture上

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = {
			 RenderEngine::Instance()->CurrentRTVPointer()};

		rtvHandle = rtvHeapHandle;


		for (uint i = 0; i < numAttachments; ++i) {

			g_pd3dDevice->CreateRenderTargetView(buffers[i].Get(), nullptr, rtvHeapHandle);

			rtvHeapHandle.ptr += g_uRtvDescriptorSize;

		}
	}


	void RenderTargetD3D12Impl::AddAttachments(Texture::Ptr* buffers, uint numAttachments, Texture::Ptr depthStencilBuffer) {
		// 创建rtv  绑定到这些texture上

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle = {
			 RenderEngine::Instance()->CurrentDSVPointer() };

		dsvHandle = dsvHeapHandle;

		g_pd3dDevice->CreateDepthStencilView( depthStencilBuffer.Get(), &ddsvd, dsvHeapHandle);


		AddAttachments(buffers, numAttachments);
	}


}
