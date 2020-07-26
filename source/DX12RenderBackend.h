#pragma once

#include "RenderBackend.h"

// TODO:�洢��ÿ�������vertex��ʾ��pso��Ϣ
// ������ָ����Щ��ָ��
// 

namespace Falcon {



	// �������Ⱦ����
	class DX12RenderBackend : public RenderBackend
	{
	private:
		/*std::map<std::string, RenderItem> _renderObjMap;
		std::map<std::string, PipeLineStateObject> _PSOMap;
		std::map<std::string, MaterialBase> _matMap;
		std::map<std::string, FrameBufferObject> _fbMap;
		std::vector<Pass> _passes;*/

		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> _mGeometries;
		std::unordered_map<std::string, ComPtr<ID3DBlob>> _mShaders;
		std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> _mPSOs;
		ComPtr<ID3D12RootSignature> _mRootSignature = nullptr;

		ComPtr<ID3D12DescriptorHeap> _mCbvHeap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _mainRTVHeap; // ����Ⱦ
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _renderTextureRTVHeap; // render Texture
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> _mDsvHeap;
		ComPtr<ID3D12DescriptorHeap> _mSrvDescriptorHeap = nullptr;

		std::vector<Pass> _passes;
		std::vector<AfterEffect> _aes;
		// DX object
		_dxgiFactory;
		_d3dDevice;
		_Fence;
		_MSAA;
		_RtvDescriptorSize;
		_DsvDescriptorSize;
		_CbvSrvUavDescriptorSize;

	public:
		DX12RenderBackend();
		void InitializePipeline();    // initialize global context
		void InitializeResources();          // ��������Դ��ʼ����������Դ����Scene::OnStart�б���ӵ�RenderBackend�У�


		// virtual methods   
		virtual void DoRender();
		virtual void DoDispatch(); // TODO: computer shader��ʵ��
	};
}

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif
