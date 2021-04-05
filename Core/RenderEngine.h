#include "../Utility/utility.h"
#include "Context.h"
namespace Falcon {
	
	


	

	class RenderEngine {
	protected:
		static RenderEngine* _selfPointer;
		std::string _outputTex;
		std::map<std::string, Resource::Ptr> _rescMap;
	public:
		virtual Resource::Ptr CreateResource();
		virtual Texture::Ptr CreateTexture();
		virtual Buffer::Ptr CreateBuffer();

		virtual void Initialize(HWND hMainWnd, int w, int h);
		virtual void FreeResources();
		virtual void Present();
		static RenderEngine* Instance();

		void AddOutput(const std::string& outputStr);
		void BindInput(std::string& targetResourceName, const std::string& sourceResourceName);   


	};
	void RenderEngine::AddOutput(const std::string& outputStr) {
		_outputTex = outputStr;
	}

	// TODO: Stupid
	void RenderEngine::BindInput(std::string& targetResourceName, const std::string& sourceResourceName) {
		targetResourceName = sourceResourceName;
	}


	RenderEngine* RenderEngine::Instance() {
		if (_selfPointer == nullptr) {
			switch (Context::Instance().GetApi())
			{
			case ApiType::D3D12:
				_selfPointer = new RenderEngineD3D12Impl();
				break;
			//case ApiType::D3D11:
			//	// 抛出异常
			//	break;
			//case ApiType::OGL:
			//	// 抛出异常
			//	break;
			//case ApiType::Vulkan:
			//	// 抛出异常
			//	break;
			//case ApiType::Nan:
			//	// 抛出异常
			//	break;
			default:
				break;
			}
		}
		return _selfPointer;
	}

	class RenderEngineD3D12Impl: public RenderEngine {
	private:
		static const uint _SwapChainBufferCount = 3;
		ComPtr<IDXGISwapChain4> _swapChain;
		ComPtr<ID3D12Resource> _pSwapChainBuffers[_SwapChainBufferCount];

		ComPtr<ID3D12GraphicsCommandList> _commandList;

		ComPtr<ID3D12CommandAllocator> _commandAllocators[FrameCount];
		ComPtr<ID3D12CommandQueue> _commandQueue;



		uint currBinding = -1;
		std::vector< ComPtr<ID3D12RootSignature>> _mRootSignatures;
		//std::vector<std::map<std::string, int>> shaderVarNameToRootSignatureSlot;
		std::vector<std::map<std::string, int>> bindingNameToRootSignatureSlot;
		std::vector<std::map<std::string, ResourceState>> shaderVarNameToBindingState;


		ComPtr<ID3D12DescriptorHeap> g_pRTVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> g_pDSVDescriptorHeap;
		

		
		/*
		// 一个renderPass维护一个root signature，一组shader对应一个管线
		// 每个shader（管线）一个space，防止因为共用root signature导致冲突
		// 通过shader反射来创建root signature（
		// 资源绑定需要在initialize阶段完成（因为要根据descriptor table进行descriptor的创建 ）
			// 一个管线 多组vertex  可能需要多次切换纹理；因此把管线的resource binding抽象出来
		
		ComPtr<ID3D12DescriptorHeap> g_pSRVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> g_pUAVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> g_pCBVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> g_pStaticDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> g_pMutableDescriptorHeap;
		//ComPtr<ID3D12DescriptorHeap> g_pDynamicDescriptorHeap;
		// TODO: 详细过程
		// 1. 创建资源（不考虑descriptor）
		// 2. 通过shader创建管线
		// 3. 通过shader 获取变量相关信息（只要知道顺序和space）   创建root signature
		// 4. 用户创建resource binding，一次性绑定资源（资源|shader var name） 
		// 5. 对每一个resource binding，根据table中的顺序，根据类型（uav srv） 为资源创建descriptor
		

		// 对于常量，用gpu address绑定到root signature


		// TODO: 资源转换与barrier
		// resource binding知道当前资源的target 状态；绑定前检查 并且转换
		// 每个资源知道自己当前的状态
		// uav barrier  ？？？ 暂时不考虑，或者要求用户自己加barrier
		
		*/
		
		
		
		
		ComPtr<ID3D12DescriptorHeap> g_pSRVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> g_pUAVDescriptorHeap;
		// 常量直接绑定

		

	public:
		RenderEngineD3D12Impl() {

		}

		virtual Resource::Ptr CreateResource();
		virtual Texture::Ptr CreateTexture();
		virtual Buffer::Ptr CreateBuffer();
		virtual void Initialize(HWND hMainWnd, int w, int h);
		virtual void FreeResources();

		virtual void Present();

		virtual void SetBindingID(uint rsid);

		bool BuildRootSignature(std::vector<ShaderObject>& ShaderObjs);
		bool BindConstantToPipeline(const std::string& constVarName, Resource::Ptr resource);
		void BindResourceBindingToPipeline(ShaderResourceBinding::Ptr srb);




		void UAVBarrier(std::vector<Resource::Ptr> pResources);

		void Resize(int w, int h);
		void ExecuteCommandList();

	};
	void RenderEngineD3D12Impl::SetBindingID(uint rsid) {
		if (rsid == -1) {
			currBinding = -1;
		}
		else {
			currBinding = rsid;
			_commandList->SetRootSignature(_mRootSignatures[rsid].Get());
		}

	}

	// 要求用户自行添加
	void RenderEngineD3D12Impl::UAVBarrier(std::vector<Resource::Ptr> pResources) {
		std::vector< CD3DX12_RESOURCE_BARRIER> barriers(pResources.size());
		for (size_t i = 0; i < pResources.size(); i++) {
			barriers[i] = CD3DX12_RESOURCE_BARRIER::UAV(pResources[i].Get());
		}
		_commandList->ResourceBarrier(pResources.size(), barriers.data());

	}
	




	//}
	// 通过shader反射的结果来构建root signature
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d12shader/nn-d3d12shader-id3d12shaderreflection  
	bool RenderEngineD3D12Impl::BuildRootSignature(std::vector<ShaderObject>& ShaderObjs) {
		// 每个space 每种类型一个parameter


		
		// varName   type   index
		std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
		rootParameters.reserve(10000);

		uint slotIndex = 0;

		std::map<std::string, Pair> parseResult;    
		for (size_t i = 0; i < numOfShader; i++) {
			auto srvTable = shaderObject->GetSRVTable();
			auto uavTable = shaderObject->GetUAVTable();

			

			auto srvtn = shaderObj->SRVTableName();
			auto uavtn = shaderObj->UAVTableName();
			bindingNameToRootSignatureSlot[currBinding][srvtn] = slotIndex++;
			bindingNameToRootSignatureSlot[currBinding][uavtn] = slotIndex++;

			CD3DX12_ROOT_PARAMETER srvPara, uavPara;
			srvPara.InitAsDescriptorTable(numRange, range);
			uavPara.InitAsDescriptorTable(numRange, range);
			rootParameters.push_back(srvPara);
			rootParameters.push_back(uavPara);


			for () {
				// for constant
				CD3DX12_ROOT_PARAMETER constPara;
				constPara.InitAsConstantBufferView(registerIndex++, space);

				bindingNameToRootSignatureSlot[currBinding][constantName] = slotIndex++;

			}
			

		}

		// TODO: target状态
		shaderVarNameToBindingState[currBinding][varName] = item.Type;


		vector<D3D12_STATIC_SAMPLER_DESC> sampler();
		int index = 0;


		
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc;
		globalRootSignatureDesc.Init(rootParameters.size(), rootParameters.data(), sampler.size(), sampler.data(), rootSignatureFlags);

		auto device = m_deviceResources->GetD3DDevice();
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error;

		ThrowIfFailed(D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error), error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);
		ThrowIfFailed(device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(_mRootSignatures[currBinding]))));
	}




	bool RenderEngineD3D12Impl::BindConstantToPipeline(const std::string& constVarName, Resource::Ptr resource) {
		if (currBinding == -1)
			return false;

		ResourceState targetRS = shaderVarNameToBindingState[currBinding][shaderVarName];

		if (targetRS != resource->CurrentState()) {
			// barrier

			resource->SetState(targetRS);
		}
		_commandList->SetGraphicsRootConstantBufferView(shaderVarNameToRootSignatureSlot[currBinding][shaderVarName], resource->VirtualGPUAddress());

		return true;
	}

	//bool RenderEngineD3D12Impl::BindScene() {
	//	// 绑定场景作为SRV
	//}
	/*
	//bool RenderEngineD3D12Impl::BindResource(const string& shaderVarName, const string&  resourceName) {

	//	return BindResource(shaderVarName, _rescMap[resourceName]);

	//}

	//bool RenderEngineD3D12Impl::BindResource(const string& shaderVarName, Resource::Ptr resource) {

	//	if (currBinding == -1)
	//		return false;

	//	ResourceState targetRS = shaderVarNameToBindingState[currBinding][shaderVarName];


	//	// TODO: 多线程情况   死锁？？    
	//	if (targetRS != resource->CurrentState()) {
	//		// barrier

	//		resource->SetState(targetRS);
	//	}
	//	//SetGraphicsRootConstantBufferView
	//	commandList->SetRootDescriptorTable(shaderVarNameToRootSignatureSlot[currBinding][shaderVarName], resource->Descriptor());

	//	return true;
	//}
	*/

	void RenderEngineD3D12Impl::BindResourceBindingToPipeline(ShaderResourceBinding::Ptr srb) {
		// 判断root signature

		for (auto bindingTable : srb) {
			_commandList->SetGraphicsRootDescriptorTable(shaderVarNameToRootSignatureSlot[currBinding][], bindingTable->HeadDescriptorHandle());

		}
		
	}

	void RenderEngineD3D12Impl::Resize(int w, int h) {

		   HRESULT hr;
		   int i;
		   DXGI_SWAP_CHAIN_DESC scDesc;

		  /* assert(g_pd3dDevice && "Device must be available!");
		   assert(g_pd3dDirectCmdAlloc && "Command allocator must be available!");
		   assert(g_pSwapChain && "Swap chain must be available!");*/

		   // Flush before changing any resources.
		   FlushCommandQueue();

		   V_RETURN(_commandList->Reset(g_pd3dDirectCmdAlloc.Get(), nullptr));

		   // Release the previous resources we will be recreating.
		   for (i = 0; i < g_iSwapChainBufferCount; ++i)
		     g_pSwapChainBuffers[i] = nullptr;
		   g_pDepthStencilBuffer = nullptr;

		   _swapChain->GetDesc(&scDesc);

		   // Resize the swap chain.
		   V_RETURN(_swapChain->ResizeBuffers(g_iSwapChainBufferCount, w, h,
		                                        g_BackBufferFormat, scDesc.Flags));

		   g_iCurrentFrameIndex = 0;

		   // RTVs
		   D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = {
		       g_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()};
		   for (i = 0; i < g_iSwapChainBufferCount; ++i) {
		     g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_pSwapChainBuffers[i]));

		     g_pd3dDevice->CreateRenderTargetView(g_pSwapChainBuffers[i].Get(), nullptr,
		                                          rtvHeapHandle);

		     rtvHeapHandle.ptr += g_uRtvDescriptorSize;
		   }

		   // Create depth-stencil buffer and the view.
		   D3D12_RESOURCE_DESC dsd;
		   dsd.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		   dsd.Alignment = 0;
		   dsd.Width = w;
		   dsd.Height = h;
		   dsd.DepthOrArraySize = 1;
		   dsd.MipLevels = 1;

		   dsd.Format = DXGI_FORMAT_R24G8_TYPELESS;
		   dsd.SampleDesc.Count = 1;
		   dsd.SampleDesc.Quality = 0;
		   dsd.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		   dsd.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		   D3D12_CLEAR_VALUE optValue;
		   optValue.Format = g_DepthStencilFormat;
		   optValue.DepthStencil.Depth = 1.0f;
		   optValue.DepthStencil.Stencil = 0;

		   D3D12_HEAP_PROPERTIES heapProps = {D3D12_HEAP_TYPE_DEFAULT,
		                                      D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		                                      D3D12_MEMORY_POOL_UNKNOWN, 0, 0};

		   V_RETURN(g_pd3dDevice->CreateCommittedResource(
		       &heapProps, D3D12_HEAP_FLAG_NONE, &dsd, D3D12_RESOURCE_STATE_COMMON,
		       &optValue, IID_PPV_ARGS(&g_pDepthStencilBuffer)));

		   // Create descriptor to mip level 0 of entire resource using the format of the
		   // resource.
		   D3D12_DEPTH_STENCIL_VIEW_DESC ddsvd;
		   ddsvd.Flags = D3D12_DSV_FLAG_NONE;
		   ddsvd.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		   ddsvd.Format = g_DepthStencilFormat;
		   ddsvd.Texture2D.MipSlice = 0;
		   g_pd3dDevice->CreateDepthStencilView(
		       g_pDepthStencilBuffer.Get(), &ddsvd,
		       g_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		   // Transition the resource from its initial state to be used as a depth
		   // buffer.
		   D3D12_RESOURCE_BARRIER dsvBarrier = {D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		                                        D3D12_RESOURCE_BARRIER_FLAG_NONE};
		   dsvBarrier.Transition.pResource = g_pDepthStencilBuffer.Get();
		   dsvBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		   dsvBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		   dsvBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		   _commandList->ResourceBarrier(1, &dsvBarrier);

		   // Execute the resize commands.
		   V_RETURN(_commandList->Close());
		   ID3D12CommandList *cmdLists[] = { _commandList.Get()};
		   _commandQueue->ExecuteCommandLists(1, cmdLists);

		   // Wait until resize is complete.
		   FlushCommandQueue();

		   // Update the viewport transform to cover the client area.
		   g_ScreenViewport.TopLeftX = 0.f;
		   g_ScreenViewport.TopLeftY = 0.f;
		   g_ScreenViewport.Width = 1.f * w;
		   g_ScreenViewport.Height = 1.f * h;
		   g_ScreenViewport.MinDepth = 0.f;
		   g_ScreenViewport.MaxDepth = 1.f;

		   g_ScreenScissorRect = {0, 0, (LONG)w, (LONG)h};

		   //return hr;
	}

	void RenderEngineD3D12Impl::Initialize(HWND hMainWnd, int w, int h) {
		HRESULT hr;
		  UINT dxgiFactoryFlags = 0;
		#ifdef _DEBUG
		  ComPtr<ID3D12Debug> debugController;
		
		  if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		    debugController->EnableDebugLayer();
		    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		  }
		#endif
		
		  ComPtr<IDXGIFactory4> factory;
		  ComPtr<IDXGIAdapter1> adapter;
		
		  V_RETURN(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));
		
		  auto AdapterSelctionPred = [](IDXGIAdapter1 *pAdapter) {
				DXGI_ADAPTER_DESC1 desc;
				pAdapter->GetDesc1(&desc);
		
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
				  // Basic Render Driver Adapter
				  return FALSE;
				} else {
				  // Check whether the adapter support dx12
				  if (FAILED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0,
											   IID_PPV_ARGS((ID3D12Device **)0)))) {
					return FALSE;
				  }
				  return TRUE;
				}
		  };
		
		  // select a adapter
		  V_RETURN(
		      GetHardwareAdpter(factory.Get(), TRUE, AdapterSelctionPred, &adapter));
		  // create device
		  V_RETURN(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
		                             IID_PPV_ARGS(&g_pd3dDevice)));
		
		  g_iFencePoint = 0;
		  V_RETURN(g_pd3dDevice->CreateFence(g_iFencePoint, D3D12_FENCE_FLAG_NONE,
		                                     IID_PPV_ARGS(&g_pd3dFence)));
		
		  g_uRtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(
		      D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		  g_uDsvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(
		      D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		  g_uCbvSrvUavDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(
		      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		  V_RETURN(CreateCommandObjects());
		  V_RETURN(CreateSwapChain(hMainWnd, w, h, factory.Get()));
		  V_RETURN(CreateRtvAndDsvDescriptorHeaps(0, 0));
		
		  //return hr;
	}


	


	// Send the command list off to the GPU for processing.
	void RenderEngineD3D12Impl::ExecuteCommandList()
	{
		ThrowIfFailed(_commandList->Close());
		ID3D12CommandList* commandLists[] = { _commandList.Get() };
		_commandQueue->ExecuteCommandLists(ARRAYSIZE(commandLists), commandLists);
	}
	void RenderEngineD3D12Impl::Present() {
		/*
		//auto canPresent  = checkIf(renderTarget, D3D12_RESOURCE_STATE_PRESENT);

		D3D12_RESOURCE_BARRIER rdBarriers[2] = { 
			{
				D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
				D3D12_RESOURCE_BARRIER_FLAG_NONE,
			},
			{
				D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
				D3D12_RESOURCE_BARRIER_FLAG_NONE,
			},
		};
		rdBarriers[0].Transition.pResource = swapChain[g_iCurrentFrameIndex].Get();
		rdBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		rdBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rdBarriers[1].Transition.pResource = outputTexturePtr.Get();
		rdBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rdBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		//g_pd3dCommandList->ResourceBarrier(2, rdBarriers);

		*/
		 // 把outputTexture的东西copy到当前的swapchain上

		D3D12_RESOURCE_BARRIER preCopyBarriers[2];
		preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(_swapChain[g_iCurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COPY_DEST);
		preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(_rescMap[_outputTex].Get(), _rescMap[_outputTex]->CurrentState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
		g_pd3dCommandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

		g_pd3dCommandList->CopyResource(_swapChain[g_iCurrentFrameIndex].Get(), _rescMap[_outputTex].Get());

		D3D12_RESOURCE_BARRIER postCopyBarriers[2];
		postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(_swapChain[g_iCurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
		//postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		g_pd3dCommandList->ResourceBarrier(1, postCopyBarriers);


		ExecuteCommandList();

		HRESULT hr;
		if (m_options & c_AllowTearing)
		{
			// Recommended to always use tearing if supported when using a sync interval of 0.
			// Note this will fail if in true 'fullscreen' mode.
			hr = _swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
		}
		else
		{
			// The first argument instructs DXGI to block until VSync, putting the application
			// to sleep until the next VSync. This ensures we don't waste any cycles rendering
			// frames that will never be displayed to the screen.
			hr = _swapChain->Present(1, 0);
		}

	}


}