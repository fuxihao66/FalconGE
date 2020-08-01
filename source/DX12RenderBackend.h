#pragma once

#include "RenderBackend.h"

// TODO:�洢��ÿ�������vertex��ʾ��pso��Ϣ
// ������ָ����Щ��ָ��
// 

namespace Falcon {

	class Material {
		//std::unordered_map<std::string, int> _slots;

		std::unordered_map<std::string, int> _offset;
	public:
		//TODO: slotֱ�� const 0��tex 1��sampler 2��uav 3
		// ������Ⱦ�׶�setGraphicsRootDescriptorTable
		int GetConSlot() {
			return 0;
		}
		int GetTexSlot() {
			return 1;
		}
		/*void AddRootSigSlot(const std::string& key, int slot) {
			_slots[key] = slot;
		}*/
		// ���ڻ�ȡ�����ĵ�ַƫ����
		void AddParaOffset(const std::string& key, int offset) {
			//_slots[key] = slot;
		}
	};

	// �������Ⱦ����
	class DX12RenderBackend : public RenderBackend
	{
	private:
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> _mGeometries;
		std::unordered_map<std::string, ComPtr<ID3DBlob>> _mShaders;
		std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> _mPSOs;
		ComPtr<ID3D12RootSignature> _mRootSignature = nullptr;
		std::unordered_map<std::string, Material> _mMats;

		/*�ײ����ݽṹ*/
		// ���� ����  ������
		ComPtr<ID3D12DescriptorHeap> _mCBVHeap = nullptr;
		// renderTarget
		ComPtr<ID3D12DescriptorHeap> _mainRTVHeap; // ����Ⱦ��ǰ�󻺳�
		ComPtr<ID3D12DescriptorHeap> _renderTextureRTVHeap; // render Texture buffer������offset������
		// ���  ģ��
		ComPtr<ID3D12DescriptorHeap> _mDSVHeap;
		// shader
		ComPtr<ID3D12DescriptorHeap> _mSRVDescriptorHeap = nullptr;
		// ֧�������д  ��������computer shader
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
