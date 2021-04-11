#include "RenderEngine.h"
//#include "ShaderObject.h"
namespace Falcon {
	
	
	RenderEngineD3D12Impl::RenderEngineD3D12Impl() {

	}

	RenderEngineD3D12Impl* RenderEngineD3D12Impl::Instance() {
		if (_selfPointer == nullptr) {
			_selfPointer = new RenderEngineD3D12Impl();
		}
		return _selfPointer;
	}

		
	void RenderEngineD3D12Impl::AddOutput(const std::string& outputStr) {
		_outputTex = outputStr;
	}

	// TODO: Stupid
	void RenderEngineD3D12Impl::BindInput(std::string& targetResourceName, const std::string& sourceResourceName) {
		targetResourceName = sourceResourceName;
	}
		
	void RenderEngineD3D12Impl::RenderEngineD3D12Impl::FreeResources() {

	}
	ComPtr<ID3D12Device> RenderEngineD3D12Impl::GetD3D12Device() {
		return _device;
	}


	ComPtr<ID3D12Device> RenderEngineD3D12Impl::GetD3D12Device() {
		return _device;
	}
	ComPtr<ID3D12GraphicsCommandList> RenderEngineD3D12Impl::GetCmdList() {
		return _d3dCommandList;
	}
	uint RenderEngineD3D12Impl::GeneratePossibleRSIndex() {
		uint r = currRSUsabeIndex;
		currRSUsabeIndex++;
		return r;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RenderEngineD3D12Impl::GetCPUTopRTVDescriptorHandle() {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), _currAvailableRTVDescriptorIndex, _RtvDescriptorSize);

	}
	D3D12_CPU_DESCRIPTOR_HANDLE RenderEngineD3D12Impl::GetCPUTopDSVDescriptorHandle() {
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), _currAvailableDSVDescriptorIndex, _DsvDescriptorSize);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE RenderEngineD3D12Impl::GetGPUTopSRVDescriptorHandle() {
		// 获取当前（可用的）descriptor handle

		return CD3DX12_GPU_DESCRIPTOR_HANDLE(_pSRVDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), _currAvailableSRVDescriptorIndex, _CbvSrvUavDescriptorSize);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE RenderEngineD3D12Impl::GetGPUTopUAVDescriptorHandle() {
		// 获取当前（可用的）descriptor handle

		return CD3DX12_GPU_DESCRIPTOR_HANDLE(_pUAVDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), _currAvailableUAVDescriptorIndex, _CbvSrvUavDescriptorSize);

	}

	void RenderEngineD3D12Impl::CreateRenderTargetView(ID3D12Resource* pResource) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), _currAvailableRTVDescriptorIndex, _RtvDescriptorSize);
		_device->CreateRenderTargetView(pResource, nullptr, handle);
		_currAvailableRTVDescriptorIndex++;
	}
	void RenderEngineD3D12Impl::CreateDepthStencilView(ID3D12Resource* pResource) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), _currAvailableDSVDescriptorIndex, _DsvDescriptorSize);
		_device->CreateDepthStencilView(pResource, nullptr, handle);
		_currAvailableDSVDescriptorIndex++;

	}
	void RenderEngineD3D12Impl::CreateShaderResourceView(ID3D12Resource* pResource) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pSRVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), _currAvailableSRVDescriptorIndex, _CbvSrvUavDescriptorSize);
		_device->CreateShaderResourceView(pResource, nullptr, handle);
		_currAvailableSRVDescriptorIndex++;
	}
	void RenderEngineD3D12Impl::CreateUnorderedAccessView(ID3D12Resource* pResource) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(_pUAVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), _currAvailableUAVDescriptorIndex, _CbvSrvUavDescriptorSize);
		_device->CreateUnorderedAccessView(pResource, nullptr, nullptr, handle);

		_currAvailableUAVDescriptorIndex++;
	}

	//void RenderEngineD3D12Impl::CreateRTPipeline() {}
	void RenderEngineD3D12Impl::CreateComputePipeline(uint bufferSize, void* bufferPointer, const string& pipelineName) {
		D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.pRootSignature = _mRootSignatures[currBinding].Get();
		psoDesc.CS = CD3DX12_SHADER_BYTECODE(bufferPointer, bufferSize);

		_device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineStates[pipelineName]));
	}
	void RenderEngineD3D12Impl::CreateGraphicsPipeline(const string& pipelineName, uint vsBufferSize, void* vsBufferPointer, uint psBufferSize, void* psBufferPointer, uint numRenderTarget, uint gsBufferSize, void* gsBufferPointer) {
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};

		D3D12_INPUT_ELEMENT_DESC gradientElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		psoDesc.InputLayout = { gradientElementDescs, _countof(gradientElementDescs) };
		psoDesc.pRootSignature = _mRootSignatures[currBinding].Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(vsBufferPointer, vsBufferSize);
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(psBufferPointer, psBufferSize);
		if (gsBufferPointer != nullptr) {
			psoDesc.GS = CD3DX12_SHADER_BYTECODE(gsBufferPointer, gsBufferSize);
		}
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = TRUE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = numRenderTarget;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		psoDesc.SampleDesc.Count = 1;

		_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineStates[pipelineName]));
	}
	/*void RenderEngineD3D12Impl::CreatePipeline(uint bufferSize, void* bufferPointer, const string& pipelineName) {
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		psoDesc.InputLayout = { gradientElementDescs, _countof(gradientElementDescs) };
		psoDesc.pRootSignature = m_rootSignature.Get();
		psoDesc.VS = CD3DX12_SHADER_BYTECODE(_gradientVS, sizeof(_gradientVS));
		psoDesc.PS = CD3DX12_SHADER_BYTECODE(_gradientPS, sizeof(_gradientPS));
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = m_intermediateRenderTargetFormat;
		psoDesc.SampleDesc.Count = 1;

		_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineStates[pipelineName]));
	}*/
	void RenderEngineD3D12Impl::SetPipeline(const std::string& pipelineName) {
		_d3dCommandList->SetPipelineState(_pipelineStates[pipelineName].Get());
	}
	void RenderEngineD3D12Impl::SetBindingID(uint rsid) {
		if (rsid == -1) {
			currBinding = -1;
		}
		else {
			currBinding = rsid;
			_d3dCommandList->SetGraphicsRootSignature(_mRootSignatures[rsid].Get());
		}

	}

	// 要求用户自行添加
	void RenderEngineD3D12Impl::UAVBarrier(std::vector<ResourceD3D12Impl::Ptr> pResources) {
		std::vector< CD3DX12_RESOURCE_BARRIER> barriers(pResources.size());
		for (size_t i = 0; i < pResources.size(); i++) {
			barriers[i] = CD3DX12_RESOURCE_BARRIER::UAV(pResources[i]->GetResourcePointer());
		}
		_d3dCommandList->ResourceBarrier(pResources.size(), barriers.data());

	}
	
	uint2 RenderEngineD3D12Impl::GetWidthHeight() {
		return uint2(_width, _height);
	}

	void RenderEngineD3D12Impl::SetRenderTargets(uint numRenderTarget, RenderTargetD3D12Impl::Ptr rt) {
		_d3dCommandList->OMSetRenderTargets(numRenderTarget, &rt->GetRTVHandle(), FALSE, nullptr);
	}
	void RenderEngineD3D12Impl::ClearRenderTargets(uint numRenderTarget, RenderTargetD3D12Impl::Ptr rt, const FLOAT ColorRGBA[4]) {
		for (int i = 0; i < numRenderTarget; i++) {
			CD3DX12_CPU_DESCRIPTOR_HANDLE tempHandle(rt->GetRTVHandle(), i, _RtvDescriptorSize);
			_d3dCommandList->ClearRenderTargetView(tempHandle, ColorRGBA, 0, nullptr);

		}
	}
	void RenderEngineD3D12Impl::SetTopology(D3D_PRIMITIVE_TOPOLOGY top) {
		_d3dCommandList->IASetPrimitiveTopology(top);

	}

	void RenderEngineD3D12Impl::SetVertexBuffers(D3D12_VERTEX_BUFFER_VIEW vb) {
		_d3dCommandList->IASetVertexBuffers(0, 1, &vb);

	}
	void RenderEngineD3D12Impl::SetIndexBuffers(D3D12_INDEX_BUFFER_VIEW ib) {
		_d3dCommandList->IASetIndexBuffer(&ib);

	}
	void RenderEngineD3D12Impl::SetViewports() {
		_d3dCommandList->RSSetViewports(1, &_ScreenViewport);
	}
	void RenderEngineD3D12Impl::SetScissorRects() {
		_d3dCommandList->RSSetScissorRects(1, &_ScreenScissorRect);

	}
	void RenderEngineD3D12Impl::DrawInstanced(uint IndexCountPerInstance, uint InstanceCount) {
		_d3dCommandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, 0, 0, 0);

	}

	void RenderEngineD3D12Impl::ReserveRootSignature(uint size) {
		_mRootSignatures.resize(size);
	}

	// 通过shader反射的结果来构建root signature
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d12shader/nn-d3d12shader-id3d12shaderreflection  
	bool RenderEngineD3D12Impl::BuildRootSignature(std::vector<ShaderObject::Ptr>& ShaderObjs) {
		// 每个space 每种类型一个parameter

		auto numOfShader = ShaderObjs.size();
		
		// varName   type   index
		std::vector<CD3DX12_ROOT_PARAMETER> rootParameters;
		rootParameters.reserve(100);

		uint slotIndex = 0;

		//std::map<std::string, Pair> parseResult;    
		for (size_t i = 0; i < numOfShader; i++) {
			auto registerIndex = 0;
			auto shaderObject = ShaderObjs[i];
			auto srvTableSize = shaderObject->GetSRVTableSize();
			auto uavTableSize = shaderObject->GetUAVTableSize();

			

			auto srvtn = shaderObject->GetSRVTableName();
			auto uavtn = shaderObject->GetUAVTableName();
			bindingNameToRootSignatureSlot[currBinding][srvtn] = slotIndex++;
			bindingNameToRootSignatureSlot[currBinding][uavtn] = slotIndex++;

			CD3DX12_ROOT_PARAMETER srvPara, uavPara;
			CD3DX12_DESCRIPTOR_RANGE ranges[2]; 
			ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, uavTableSize, 0, shaderObject->GetSpace());
			ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, srvTableSize, 0, shaderObject->GetSpace());


			srvPara.InitAsDescriptorTable(1, &ranges[0]);
			uavPara.InitAsDescriptorTable(1, &ranges[1]);
			rootParameters.push_back(srvPara);
			rootParameters.push_back(uavPara);


			for (int bindPoint = 0; bindPoint < shaderObject->GetConstantBinding().size(); bindPoint++) {
				// for constant
				CD3DX12_ROOT_PARAMETER constPara;
				constPara.InitAsConstantBufferView(registerIndex++, shaderObject->GetSpace());

				bindingNameToRootSignatureSlot[currBinding][shaderObject->GetConstantBinding()[bindPoint]] = slotIndex++;

			}
			

		}

		//// TODO: target状态
		//shaderVarNameToBindingState[currBinding][varName] = item.Type;


		vector<D3D12_STATIC_SAMPLER_DESC> sampler;
		int index = 0;


		
		CD3DX12_ROOT_SIGNATURE_DESC globalRootSignatureDesc;
		globalRootSignatureDesc.Init(rootParameters.size(), rootParameters.data(), sampler.size(), sampler.data());

		
		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> error;

		D3D12SerializeRootSignature(&globalRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error), error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr;
		_device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(_mRootSignatures[currBinding])));

		return true;
	}



	bool RenderEngineD3D12Impl::BindConstantToComputePipeline(const std::string& constVarName, ResourceD3D12Impl::Ptr resource) {
		if (currBinding == -1)
			return false;

		//ResourceState targetRS = shaderVarNameToBindingState[currBinding][constVarName];

		if (D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER != resource->GetCurrentState()) {
			// barrier

			resource->SetState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}
		_d3dCommandList->SetComputeRootConstantBufferView(bindingNameToRootSignatureSlot[currBinding][constVarName], resource->GetResourcePointer()->GetGPUVirtualAddress());

		return true;
	}






	bool RenderEngineD3D12Impl::BindConstantToGraphicsPipeline(const std::string& constVarName, ResourceD3D12Impl::Ptr resource) {
		if (currBinding == -1)
			return false;

		//ResourceState targetRS = shaderVarNameToBindingState[currBinding][constVarName];

		if (D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER != resource->GetCurrentState()) {
			// barrier

			resource->SetState(D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		}
		_d3dCommandList->SetGraphicsRootConstantBufferView(bindingNameToRootSignatureSlot[currBinding][constVarName], resource->GetResourcePointer()->GetGPUVirtualAddress());

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

	void RenderEngineD3D12Impl::BindResourceBindingToComputePipeline(ShaderResourceBindingD3D12Impl::Ptr srb) {
		// 判断root signature
		srb->CheckResourceState();


		_d3dCommandList->SetComputeRootDescriptorTable(bindingNameToRootSignatureSlot[currBinding][srb->GetSRVTableName()], srb->GetSRVBindingHeadDescriptor());
		_d3dCommandList->SetComputeRootDescriptorTable(bindingNameToRootSignatureSlot[currBinding][srb->GetUAVTableName()], srb->GetUAVBindingHeadDescriptor());

		// 资源检查和转换？？
	}
	void RenderEngineD3D12Impl::BindResourceBindingToGraphicsPipeline(ShaderResourceBindingD3D12Impl::Ptr srb) {
		// 判断root signature
		srb->CheckResourceState();


		_d3dCommandList->SetGraphicsRootDescriptorTable(bindingNameToRootSignatureSlot[currBinding][srb->GetSRVTableName()], srb->GetSRVBindingHeadDescriptor());
		_d3dCommandList->SetGraphicsRootDescriptorTable(bindingNameToRootSignatureSlot[currBinding][srb->GetUAVTableName()], srb->GetUAVBindingHeadDescriptor());

		// 资源检查和转换？？
	}

	// 因为必须确保
	void RenderEngineD3D12Impl::BeginRecording() {
		_d3dDirectCmdAlloc[_iCurrentFrameIndex]->Reset();

		
		_d3dCommandList->Reset(_d3dDirectCmdAlloc[_iCurrentFrameIndex].Get(), nullptr);
	}

	void RenderEngineD3D12Impl::EndRecording() {
		_d3dCommandList->Close();

		// Execute the command list.
		ID3D12CommandList* ppCommandLists[] = { _d3dCommandList.Get() };
		_d3dCommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	}

	void RenderEngineD3D12Impl::FlushCommandQueue() {

		  HRESULT hr;
		  _iFencePoint += 1;

		  _d3dCommandQueue->Signal(_pd3dFence.Get(), _iFencePoint);

		  if (_pd3dFence->GetCompletedValue() != _iFencePoint) {
			if (!_hFenceEvent)
				_hFenceEvent = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);

			_pd3dFence->SetEventOnCompletion(_iFencePoint, _hFenceEvent);

			WaitForSingleObject(_hFenceEvent, INFINITE);
		  }
	}


	void RenderEngineD3D12Impl::Resize(int w, int h) {
		_width = w;
		_height = h;

		   HRESULT hr;
		   int i;
		   DXGI_SWAP_CHAIN_DESC scDesc;

		  /* assert(_device && "Device must be available!");
		   assert(_d3dDirectCmdAlloc && "Command allocator must be available!");
		   assert(_swapChain && "Swap chain must be available!");*/

		   // Flush before changing any resources.
		   FlushCommandQueue();
		   _iCurrentFrameIndex = 0;

		   _d3dCommandList->Reset(_d3dDirectCmdAlloc[_iCurrentFrameIndex].Get(), nullptr);

		   // Release the previous resources we will be recreating.
		   for (i = 0; i < _SwapChainBufferCount; ++i)
		     _pSwapChainBuffers[i] = nullptr;
		   _pDepthStencilBuffer = nullptr;

		   _swapChain->GetDesc(&scDesc);

		   // Resize the swap chain.
		   _swapChain->ResizeBuffers(_SwapChainBufferCount, w, h,
		                                        _BackBufferFormat, scDesc.Flags);


		   // RTVs
		   D3D12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle = {
		       _pRTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()};
		   for (i = 0; i < _SwapChainBufferCount; ++i) {
		     _swapChain->GetBuffer(i, IID_PPV_ARGS(&_pSwapChainBuffers[i]));

		     _device->CreateRenderTargetView(_pSwapChainBuffers[i].Get(), nullptr,
		                                          rtvHeapHandle);

		     rtvHeapHandle.ptr += _RtvDescriptorSize;
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
		   optValue.Format = _DepthStencilFormat;
		   optValue.DepthStencil.Depth = 1.0f;
		   optValue.DepthStencil.Stencil = 0;

		   D3D12_HEAP_PROPERTIES heapProps = {D3D12_HEAP_TYPE_DEFAULT,
		                                      D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		                                      D3D12_MEMORY_POOL_UNKNOWN, 0, 0};

		   _device->CreateCommittedResource(
		       &heapProps, D3D12_HEAP_FLAG_NONE, &dsd, D3D12_RESOURCE_STATE_COMMON,
		       &optValue, IID_PPV_ARGS(&_pDepthStencilBuffer));

		   // Create descriptor to mip level 0 of entire resource using the format of the
		   // resource.
		   D3D12_DEPTH_STENCIL_VIEW_DESC ddsvd;
		   ddsvd.Flags = D3D12_DSV_FLAG_NONE;
		   ddsvd.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		   ddsvd.Format = _DepthStencilFormat;
		   ddsvd.Texture2D.MipSlice = 0;
		   _device->CreateDepthStencilView(
		       _pDepthStencilBuffer.Get(), &ddsvd,
		       _pDSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

		   // Transition the resource from its initial state to be used as a depth
		   // buffer.
		   D3D12_RESOURCE_BARRIER dsvBarrier = {D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		                                        D3D12_RESOURCE_BARRIER_FLAG_NONE};
		   dsvBarrier.Transition.pResource = _pDepthStencilBuffer.Get();
		   dsvBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		   dsvBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
		   dsvBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		   _d3dCommandList->ResourceBarrier(1, &dsvBarrier);

		   // Execute the resize commands.
		   _d3dCommandList->Close();
		   ID3D12CommandList *cmdLists[] = { _d3dCommandList.Get()};
		   _d3dCommandQueue->ExecuteCommandLists(1, cmdLists);

		   // Wait until resize is complete.
		   FlushCommandQueue();

		   // Update the viewport transform to cover the client area.
		   _ScreenViewport.TopLeftX = 0.f;
		   _ScreenViewport.TopLeftY = 0.f;
		   _ScreenViewport.Width = 1.f * w;
		   _ScreenViewport.Height = 1.f * h;
		   _ScreenViewport.MinDepth = 0.f;
		   _ScreenViewport.MaxDepth = 1.f;

		   _ScreenScissorRect = {0, 0, (LONG)w, (LONG)h};

		   //return hr;
	}


	


	ResourceD3D12Impl::Ptr RenderEngineD3D12Impl::GetResource(const string& resName) {
		return _rescMap[resName];
	}

	void RenderEngineD3D12Impl::AddResourceToMap(const string& resName, ResourceD3D12Impl::Ptr resource) {
		_rescMap[resName] = resource;
	}

	HRESULT RenderEngineD3D12Impl::Initialize(HWND hMainWnd, int w, int h) {

		_width = w;
		_height = h;

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
		  // create _device
		  V_RETURN(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
		                             IID_PPV_ARGS(&_device)));
		
		  _iFencePoint = 0;
		  V_RETURN(_device->CreateFence(_iFencePoint, D3D12_FENCE_FLAG_NONE,
		                                     IID_PPV_ARGS(&_pd3dFence)));
		
		  _RtvDescriptorSize = _device->GetDescriptorHandleIncrementSize(
		      D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		  _DsvDescriptorSize = _device->GetDescriptorHandleIncrementSize(
		      D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		  _CbvSrvUavDescriptorSize = _device->GetDescriptorHandleIncrementSize(
		      D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		
		  V_RETURN(CreateCommandObjects());
		  V_RETURN(CreateSwapChain(hMainWnd, w, h, factory.Get()));
		  V_RETURN(CreateDescriptorHeaps());
		
		  return hr;
	}
	HRESULT RenderEngineD3D12Impl::CreateCommandObjects() {
		  HRESULT hr;
		  D3D12_COMMAND_QUEUE_DESC dqd = {};
		  dqd.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		  dqd.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		  V_RETURN(_device->CreateCommandQueue(&dqd,
													IID_PPV_ARGS(&_d3dCommandQueue)));

		  for (int i = 0; i < _SwapChainBufferCount; i++) {
			  V_RETURN(_device->CreateCommandAllocator(
				  D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_d3dDirectCmdAlloc[i])));
		  }
		  

		  V_RETURN(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
			  _d3dDirectCmdAlloc[0].Get(), nullptr,
												   IID_PPV_ARGS(&_d3dCommandList)));

		  _d3dCommandList->Close();

		  return hr;
	}
	HRESULT RenderEngineD3D12Impl::GetHardwareAdpter(_In_ IDXGIFactory1 *pFactory,
                  _In_ BOOL bRequestHighPerformanceAdapter,
                  _In_ BOOL (*AdapterSelectionCallback)(IDXGIAdapter1 *),
                  _Out_ IDXGIAdapter1 **ppAdapter) {
		  ComPtr<IDXGIFactory6> pFactory6;
		  ComPtr<IDXGIAdapter1> adapter;
		  HRESULT hr = E_FAIL;

		  *ppAdapter = nullptr;

		  if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&pFactory6)))) {
			for (int adapterIndex = 0;
				 DXGI_ERROR_NOT_FOUND != pFactory6->EnumAdapterByGpuPreference(
											 adapterIndex,
											 bRequestHighPerformanceAdapter
												 ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
												 : DXGI_GPU_PREFERENCE_UNSPECIFIED,
											 IID_PPV_ARGS(&adapter));
				 ++adapterIndex) {

			  if (AdapterSelectionCallback(adapter.Get())) {
				*ppAdapter = adapter.Get();
				adapter->AddRef();
				hr = S_OK;
				break;
			  }
			}
		  } else {
			for (int adapterIndex = 0; DXGI_ERROR_NOT_FOUND !=
									   pFactory->EnumAdapters1(adapterIndex, &adapter);
				 ++adapterIndex) {
			  if (AdapterSelectionCallback(adapter.Get())) {
				*ppAdapter = adapter.Get();
				adapter->AddRef();
				hr = S_OK;
				break;
			  }
			}
		  }

		  return hr;
	}
	HRESULT RenderEngineD3D12Impl::CreateSwapChain(HWND hwnd, int width, int height,
                        IDXGIFactory4 *pDXGIFactory) {

		  HRESULT hr;
		  DXGI_SWAP_CHAIN_DESC dscd;

		  // Release the previous Swap Chain so we can re-create new ones.
		  _swapChain = nullptr;

		  dscd.BufferDesc.Width = width;
		  dscd.BufferDesc.Height = height;
		  dscd.BufferDesc.RefreshRate.Numerator = 60;
		  dscd.BufferDesc.RefreshRate.Denominator = 1;
		  dscd.BufferDesc.Format = _BackBufferFormat;
		  dscd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		  dscd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		  dscd.SampleDesc.Count = 1;
		  dscd.SampleDesc.Quality = 0;
		  dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		  dscd.BufferCount = _SwapChainBufferCount;
		  dscd.OutputWindow = hwnd;
		  dscd.Windowed = TRUE;
		  dscd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		  dscd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
					   DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;


		  ComPtr<IDXGISwapChain> swapChain1;
		  

		  pDXGIFactory->CreateSwapChain(_d3dCommandQueue.Get(), &dscd, &swapChain1);

		  swapChain1.As(&_swapChain);
		  /*pDXGIFactory->CreateSwapChain(_d3dCommandQueue.Get(), &dscd,
												 &_swapChain);*/

		  return hr;
}

	HRESULT RenderEngineD3D12Impl::CreateDescriptorHeaps() {
		  HRESULT hr;

		  D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
		  rtvHeapDesc.NumDescriptors = _SwapChainBufferCount;
		  rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		  rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		  rtvHeapDesc.NodeMask = 0;
		  V_RETURN(_device->CreateDescriptorHeap(
			  &rtvHeapDesc, IID_PPV_ARGS(&_pRTVDescriptorHeap)));

		  D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
		  dsvHeapDesc.NumDescriptors = 1;
		  dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		  dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		  dsvHeapDesc.NodeMask = 0;
		  V_RETURN(_device->CreateDescriptorHeap(
			  &dsvHeapDesc, IID_PPV_ARGS(&_pDSVDescriptorHeap)));

		  D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc;
		  srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		  srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		  srvHeapDesc.NodeMask = 0;
		  V_RETURN(_device->CreateDescriptorHeap(
			  &srvHeapDesc, IID_PPV_ARGS(&_pSRVDescriptorHeap)));

		  D3D12_DESCRIPTOR_HEAP_DESC uavHeapDesc;
		  uavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		  uavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		  uavHeapDesc.NodeMask = 0;
		  V_RETURN(_device->CreateDescriptorHeap(
			  &uavHeapDesc, IID_PPV_ARGS(&_pUAVDescriptorHeap)));


		  return hr;
}


	// Send the command list off to the GPU for processing.
	void RenderEngineD3D12Impl::ExecuteCommandList()
	{
		_d3dCommandList->Close();
		ID3D12CommandList* commandLists[] = { _d3dCommandList.Get() };
		_d3dCommandQueue->ExecuteCommandLists(ARRAYSIZE(commandLists), commandLists);
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
		rdBarriers[0].Transition.pResource = swapChain[_iCurrentFrameIndex].Get();
		rdBarriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		rdBarriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rdBarriers[1].Transition.pResource = outputTexturePtr.Get();
		rdBarriers[1].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		rdBarriers[1].Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		//_d3dCommandList->ResourceBarrier(2, rdBarriers);

		*/
		 // 把outputTexture的东西copy到当前的swapchain上

		D3D12_RESOURCE_BARRIER preCopyBarriers[2];
		preCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(_pSwapChainBuffers[_iCurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST);
		preCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(_rescMap[_outputTex]->GetResourcePointer(), _rescMap[_outputTex]->GetCurrentState(), D3D12_RESOURCE_STATE_COPY_SOURCE);
		_d3dCommandList->ResourceBarrier(ARRAYSIZE(preCopyBarriers), preCopyBarriers);

		_d3dCommandList->CopyResource(_pSwapChainBuffers[_iCurrentFrameIndex].Get(), _rescMap[_outputTex]->GetResourcePointer());

		D3D12_RESOURCE_BARRIER postCopyBarriers[2];
		postCopyBarriers[0] = CD3DX12_RESOURCE_BARRIER::Transition(_pSwapChainBuffers[_iCurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT);
		//postCopyBarriers[1] = CD3DX12_RESOURCE_BARRIER::Transition(m_raytracingOutput.Get(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

		_d3dCommandList->ResourceBarrier(1, postCopyBarriers);


		ExecuteCommandList();

		HRESULT hr;
		hr = _swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);

		_iCurrentFrameIndex = (_iCurrentFrameIndex + 1) % _SwapChainBufferCount;
		//if (m_options & c_AllowTearing)
		//{
		//	// Recommended to always use tearing if supported when using a sync interval of 0.
		//	// Note this will fail if in true 'fullscreen' mode.
		//	hr = _swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
		//}
		//else
		//{
		//	// The first argument instructs DXGI to block until VSync, putting the application
		//	// to sleep until the next VSync. This ensures we don't waste any cycles rendering
		//	// frames that will never be displayed to the screen.
		//	hr = _swapChain->Present(1, 0);
		//}

	}


}