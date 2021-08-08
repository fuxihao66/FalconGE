#pragma once
#include "../Utility/utility.h"

#include "RenderEngine.h"
#include "Resource.h"
using std::shared_ptr;


namespace Falcon {
	/*class RenderTarget {

	public:
		using Ptr = shared_ptr<RenderTarget>;
		virtual void AddAttachments(Texture::Ptr* buffers, uint numAttachments);
		virtual void AddAttachments(Texture::Ptr* buffers, uint numAttachments, Texture::Ptr depthStencilBuffer);
	};*/


	//class RenderTargetD3D12Impl : public RenderTarget {
	class RenderTargetD3D12Impl  {
	private:
		int _numAttachments;
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	public:
		using Ptr = shared_ptr<RenderTargetD3D12Impl>;

		/*void AddAttachments(const vector<TextureD3D12Impl::Ptr>& buffers);
		void AddAttachments(const vector<TextureD3D12Impl::Ptr>& buffers, TextureD3D12Impl::Ptr depthStencilBuffer);*/

		D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() {
			return rtvHandle;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() {
			return dsvHandle;
		}
		void AddAttachments(const vector<TextureD3D12Impl::Ptr>& buffers) {
			// 创建rtv  绑定到这些texture上
			auto numAttachments = buffers.size();

			rtvHandle = {
				 RenderEngineD3D12Impl::Instance()->GetCPUTopRTVDescriptorHandle() };

			//rtvHandle = rtvHeapHandle;


			for (uint i = 0; i < numAttachments; ++i) {

				/*g_pd3dDevice->CreateRenderTargetView(buffers[i]->GetResourcePointer(), nullptr, rtvHeapHandle);

				rtvHeapHandle.ptr += g_uRtvDescriptorSize;*/
				RenderEngineD3D12Impl::Instance()->CreateRenderTargetView(buffers[i]->GetResourcePointer());

			}
		}


		void AddAttachments(const vector<TextureD3D12Impl::Ptr>& buffers, TextureD3D12Impl::Ptr depthStencilBuffer) {
			// 创建rtv  绑定到这些texture上

			dsvHandle = {
				 RenderEngineD3D12Impl::Instance()->GetCPUTopDSVDescriptorHandle() };

			//dsvHandle = dsvHeapHandle;

			RenderEngineD3D12Impl::Instance()->CreateDepthStencilView(depthStencilBuffer->GetResourcePointer());

			//g_pd3dDevice->CreateDepthStencilView( depthStencilBuffer->GetResourcePointer(), &ddsvd, dsvHeapHandle);


			AddAttachments(buffers);
		}

	};
	

	


}
