#pragma once
#include "../Utility/utility.h"
#include "Context.h"
#include "Resource.h"
#include "ShaderObject.h"
#include "RenderTarget.h"
//#include "ShaderObject.h"
namespace Falcon {
	
	

	/*
	{
		//class RenderEngine {
	//protected:
	//	static RenderEngine* _selfPointer;
	//	std::string _outputTex;
	//	std::map<std::string, Resource::Ptr> _rescMap;
	//public:
	//	virtual Resource::Ptr CreateResource();
	//	virtual Texture::Ptr CreateTexture();
	//	virtual Buffer::Ptr CreateBuffer();

	//	virtual void Initialize(HWND hMainWnd, int w, int h);
	//	virtual void FreeResources();
	//	virtual void Present();
	//	static RenderEngine* Instance();

	//	void AddOutput(const std::string& outputStr);
	//	void BindInput(std::string& targetResourceName, const std::string& sourceResourceName);   


	//};
	//void RenderEngine::AddOutput(const std::string& outputStr) {
	//	_outputTex = outputStr;
	//}

	//// TODO: Stupid
	//void RenderEngine::BindInput(std::string& targetResourceName, const std::string& sourceResourceName) {
	//	targetResourceName = sourceResourceName;
	//}


	//RenderEngine* RenderEngine::Instance() {
	//	if (_selfPointer == nullptr) {
	//		switch (Context::Instance().GetApi())
	//		{
	//		case ApiType::D3D12:
	//			_selfPointer = new RenderEngineD3D12Impl();
	//			break;
	//		//case ApiType::D3D11:
	//		//	// 抛出异常
	//		//	break;
	//		//case ApiType::OGL:
	//		//	// 抛出异常
	//		//	break;
	//		//case ApiType::Vulkan:
	//		//	// 抛出异常
	//		//	break;
	//		//case ApiType::Nan:
	//		//	// 抛出异常
	//		//	break;
	//		default:
	//			break;
	//		}
	//	}
	//	return _selfPointer;
	//}
	}
	*/
	

	//class RenderEngineD3D12Impl: public RenderEngine {
	class RenderEngineD3D12Impl  {
	private:





		/*static HRESULT
			GetHardwareAdpter(_In_ IDXGIFactory1* pFactory,
				_In_ BOOL bRequestHighPerformanceAdapter,
				_In_ BOOL(*AdapterSelectionCallback)(IDXGIAdapter1*),
				_Out_ IDXGIAdapter1** ppAdapter);
		static HRESULT CreateCommandObjects();
		static HRESULT CreateSwapChain(HWND hwnd, int width, int height,
			IDXGIFactory4* pDXGIFactory);
		static HRESULT ResizeRenderedBuffers(int width, int height);
		static HRESULT CreateRtvAndDsvDescriptorHeaps(int extraRtvCount,
			int extraDsvCount);
		static VOID FlushCommandQueue();*/


		ComPtr<ID3D12Device> _device;
		INT64 _iFencePoint;
		ComPtr<ID3D12Fence> _pd3dFence;
		HANDLE _hFenceEvent;

		UINT _RtvDescriptorSize;
		UINT _DsvDescriptorSize;
		UINT _CbvSrvUavDescriptorSize;

		static const uint _SwapChainBufferCount = 3;

		// Command queue
		ComPtr<ID3D12CommandQueue> _d3dCommandQueue;
		ComPtr<ID3D12GraphicsCommandList> _d3dCommandList;
		ComPtr<ID3D12CommandAllocator> _d3dDirectCmdAlloc[_SwapChainBufferCount];

		static constexpr DXGI_FORMAT _BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static constexpr DXGI_FORMAT _DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;



		static D3D12_VIEWPORT _ScreenViewport;
		static D3D12_RECT _ScreenScissorRect;




		static RenderEngineD3D12Impl* _selfPointer;
		std::string _outputTex;
		std::map<std::string, ResourceD3D12Impl::Ptr> _rescMap;
		
		uint _width;
		uint _height;

		ComPtr<IDXGISwapChain4> _swapChain;
		ComPtr<ID3D12Resource> _pSwapChainBuffers[_SwapChainBufferCount];
		ComPtr<ID3D12Resource> _pDepthStencilBuffer;

		/*ComPtr<ID3D12GraphicsCommandList> _commandList;
		ComPtr<ID3D12CommandAllocator> _commandAllocators[FrameCount];
		ComPtr<ID3D12CommandQueue> _commandQueue;*/



		uint currBinding = -1;
		uint currRSUsabeIndex = 0;
		std::vector< ComPtr<ID3D12RootSignature>> _mRootSignatures;
		//std::vector<std::map<std::string, int>> shaderVarNameToRootSignatureSlot;
		std::vector<std::map<std::string, int>> bindingNameToRootSignatureSlot;
		std::vector<std::map<std::string, ResourceState>> shaderVarNameToBindingState;

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
		
		
		ComPtr<ID3D12DescriptorHeap> _pRTVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> _pDSVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> _pSRVDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> _pUAVDescriptorHeap;
		// 常量直接绑定


		void FlushCommandQueue();
		HRESULT CreateCommandObjects();
		HRESULT CreateDescriptorHeaps();
		HRESULT CreateSwapChain(HWND hwnd, int width, int height, IDXGIFactory4* pDXGIFactory);
		HRESULT GetHardwareAdpter(_In_ IDXGIFactory1* pFactory,
			_In_ BOOL bRequestHighPerformanceAdapter,
			_In_ BOOL(*AdapterSelectionCallback)(IDXGIAdapter1*),
			_Out_ IDXGIAdapter1** ppAdapter);


		uint _currAvailableRTVDescriptorIndex = 0;
		uint _currAvailableDSVDescriptorIndex = 0;
		uint _currAvailableSRVDescriptorIndex = 0;
		uint _currAvailableUAVDescriptorIndex = 0;

		map<string, ComPtr<ID3D12PipelineState>> _pipelineStates;

		uint _iCurrentFrameIndex;
	public:
		RenderEngineD3D12Impl();
		static RenderEngineD3D12Impl* Instance();
		void AddOutput(const std::string& outputStr);
		// TODO: Stupid
		void BindInput(std::string& targetResourceName, const std::string& sourceResourceName);
		virtual HRESULT Initialize(HWND hMainWnd, int w, int h);
		virtual void FreeResources();
		virtual void Present();
		virtual void SetBindingID(uint rsid);
		bool BuildRootSignature(std::vector<ShaderObject::Ptr>& ShaderObjs);
		bool BindConstantToComputePipeline(const std::string& constVarName, ResourceD3D12Impl::Ptr resource);
		bool BindConstantToGraphicsPipeline(const std::string& constVarName, ResourceD3D12Impl::Ptr resource);
		void BindResourceBindingToComputePipeline(ShaderResourceBindingD3D12Impl::Ptr srb);
		void BindResourceBindingToGraphicsPipeline(ShaderResourceBindingD3D12Impl::Ptr srb);

		void CreateComputePipeline(uint bufferSize, void* bufferPointer, const string& pipelineName);
		void CreateGraphicsPipeline(const string& pipelineName, uint vsBufferSize, void* vsBufferPointer, uint psBufferSize, void* psBufferPointer, uint numRenderTarget = 1, uint gsBufferSize = 0, void* gsBufferPointer = nullptr);
		//void CreatePipeline(uint bufferSize, void* bufferPointer, const string& pipelineName);
		void SetPipeline(const std::string& pipelineName);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUTopRTVDescriptorHandle();
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUTopDSVDescriptorHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUTopSRVDescriptorHandle();
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUTopUAVDescriptorHandle();


		ComPtr<ID3D12Device> GetD3D12Device();
		ComPtr<ID3D12GraphicsCommandList> GetCmdList();

		void CreateRenderTargetView(ID3D12Resource* pResource);
		void CreateDepthStencilView(ID3D12Resource* pResource);
		void CreateShaderResourceView(ID3D12Resource* pResource);
		void CreateUnorderedAccessView(ID3D12Resource* pResource);

		void UAVBarrier(std::vector<ResourceD3D12Impl::Ptr> pResources);

		void Resize(int w, int h);
		void ExecuteCommandList();

		void AddResourceToMap(const string& resName, ResourceD3D12Impl::Ptr resource);

		
		uint GeneratePossibleRSIndex();
		void ReserveRootSignature(uint size);

		uint2 GetWidthHeight();

		void SetRenderTargets(uint numRenderTarget, RenderTargetD3D12Impl::Ptr rt);
		void ClearRenderTargets(uint numRenderTarget, RenderTargetD3D12Impl::Ptr rt, const FLOAT ColorRGBA[4]);
		void SetVertexBuffers(D3D12_VERTEX_BUFFER_VIEW vb);
		void SetIndexBuffers(D3D12_INDEX_BUFFER_VIEW ib);
		void SetTopology(D3D_PRIMITIVE_TOPOLOGY);

		void SetViewports();
		void SetScissorRects();

		void DrawInstanced(uint IndexCountPerInstance, uint InstanceCount);

		void BeginRecording();
		void EndRecording();


		ResourceD3D12Impl::Ptr GetResource(const string& resName);
	};





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

	
	

	//void CreateRTPipeline(uint bufferSize, void* bufferPointer, const string& rayGenName, vector<string>& hitGroupName, vector<string>& missGroupName, const string& pipelineName) {
		//	CD3DX12_STATE_OBJECT_DESC raytracingPipeline{ D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE };


		//	auto lib = raytracingPipeline.CreateSubobject<CD3DX12_DXIL_LIBRARY_SUBOBJECT>();
		//	D3D12_SHADER_BYTECODE libdxil = CD3DX12_SHADER_BYTECODE(bufferPointer, bufferSize);
		//	lib->SetDXILLibrary(&libdxil);
		//	// Define which shader exports to surface from the library.
		//	// If no shader exports are defined for a DXIL library subobject, all shaders will be surfaced.
		//	// In this sample, this could be ommited for convenience since the sample uses all shaders in the library. 
		//	{
		//		lib->DefineExport(rayGenName);
		//		lib->DefineExport(hitGroupName);
		//		lib->DefineExport(missGroupName);
		//	}

		//	// Triangle hit group
		//	// A hit group specifies closest hit, any hit and intersection shaders to be executed when a ray intersects the geometry's triangle/AABB.
		//	// In this sample, we only use triangle geometry with a closest hit shader, so others are not set.
		//	auto hitGroup = raytracingPipeline.CreateSubobject<CD3DX12_HIT_GROUP_SUBOBJECT>();
		//	hitGroup->SetClosestHitShaderImport(c_closestHitShaderName);
		//	hitGroup->SetHitGroupExport(c_hitGroupName);
		//	hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

		//	// Shader config
		//	// Defines the maximum sizes in bytes for the ray payload and attribute structure.
		//	auto shaderConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_SHADER_CONFIG_SUBOBJECT>();
		//	UINT payloadSize = sizeof(XMFLOAT4);    // float4 pixelColor
		//	UINT attributeSize = sizeof(XMFLOAT2);  // float2 barycentrics
		//	shaderConfig->Config(payloadSize, attributeSize);

		//	// Local root signature and shader association
		//	// This is a root signature that enables a shader to have unique arguments that come from shader tables.
		//	CreateLocalRootSignatureSubobjects(&raytracingPipeline);

		//	auto globalRootSignature = raytracingPipeline.CreateSubobject<CD3DX12_GLOBAL_ROOT_SIGNATURE_SUBOBJECT>();
		//	globalRootSignature->SetRootSignature(m_raytracingGlobalRootSignature.Get());

		//	auto pipelineConfig = raytracingPipeline.CreateSubobject<CD3DX12_RAYTRACING_PIPELINE_CONFIG_SUBOBJECT>();
		//	UINT maxRecursionDepth = 1; // ~ primary rays only. 
		//	pipelineConfig->Config(maxRecursionDepth);

		//	// Create the state object.
		//	m_dxrDevice->CreateStateObject(raytracingPipeline, IID_PPV_ARGS(&m_dxrStateObject));
		//}
}