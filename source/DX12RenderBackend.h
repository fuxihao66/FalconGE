#pragma once

#include "RenderBackend.h"

// TODO:存储了每个物体的vertex表示和pso信息
// 包括了指向这些的指针
// 

namespace Falcon {

	class Material {
		//std::unordered_map<std::string, int> _slots;

		std::unordered_map<std::string, int> _offset;
	public:
		//TODO: slot直接 const 0；tex 1；sampler 2；uav 3
		// 用于渲染阶段setGraphicsRootDescriptorTable
		int GetConSlot() {
			return 0;
		}
		int GetTexSlot() {
			return 1;
		}
		/*void AddRootSigSlot(const std::string& key, int slot) {
			_slots[key] = slot;
		}*/
		// 用于获取变量的地址偏移量
		void AddParaOffset(const std::string& key, int offset) {
			//_slots[key] = slot;
		}
	};

	// 具体的渲染引擎
	class DX12RenderBackend : public RenderBackend
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> _mGeometries;
		std::unordered_map<std::string, ComPtr<ID3DBlob>> _mShaders;
		std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> _mPSOs;
		ComPtr<ID3D12RootSignature> _mRootSignature = nullptr;
		std::unordered_map<std::string, Material> _mMats;

		/*底层数据结构*/
		// 矩阵 向量  浮点数
		ComPtr<ID3D12DescriptorHeap> _mCBVHeap = nullptr;
		// renderTarget
		ComPtr<ID3D12DescriptorHeap> _mainRTVHeap; // 主渲染，前后缓冲
		ComPtr<ID3D12DescriptorHeap> _renderTextureRTVHeap; // render Texture buffer，根据offset来索引
		// 深度  模板
		ComPtr<ID3D12DescriptorHeap> _mDSVHeap;
		// shader
		ComPtr<ID3D12DescriptorHeap> _mSRVDescriptorHeap = nullptr;
		// 支持随机读写  可以用于computer shader
		ComPtr<ID3D12DescriptorHeap> _mUAVDescriptorHeap = nullptr;

		
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
		void InitializeResources();          // 初步的资源初始化（场景资源会在Scene::OnStart中被添加到RenderBackend中）


		// virtual methods   
		virtual void DoRender();
		virtual void DoDispatch(); // TODO: computer shader的实现
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
