#pragma once

#include "../Utility/utility.h"
#include "Resource.h"
#include "RenderEngine.h"
using std::shared_ptr;
using std::vector;
using std::map;
using std::string;


class ShaderResourceBinding {

};

namespace Falcon {
	class ShaderResourceBindingD3D12Impl : public ShaderResourceBinding {
	public:
		const std::string _srvTableName;
		const std::string _uavTableName;

		vector<std::string> _srvResourceBinding;
		vector<std::string> _uavResourceBinding;

		D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE uavHandle;

		map<string, ResourceD3D12Impl::Ptr> _binding;
	public:
		using Ptr = shared_ptr<ShaderResourceBindingD3D12Impl>;
		


		ShaderResourceBindingD3D12Impl(const std::string& sn, const std::string& un, const vector<std::string>& srvResource, const vector<std::string>& uavResource)
			: _srvTableName(sn), _uavTableName(un)
		{
			_srvResourceBinding = srvResource;
			_uavResourceBinding = uavResource;


			
		}
		/*void BindSRB() {

		}*/

		

		void BindResource(const map<string, ResourceD3D12Impl::Ptr>& binding) 
		{
			_binding = binding;

			//bool hasSetSRVGPUHandle = false;


			srvHandle = RenderEngineD3D12Impl::Instance()->GetGPUTopSRVDescriptorHandle();
			uavHandle = RenderEngineD3D12Impl::Instance()->GetGPUTopUAVDescriptorHandle();
			// 创建descriptor
			//auto tempSRVHandle = RenderEngineD3D12Impl::Instance()->GetCurrentTopSRVDescriptorHandle(); 
			for (uint i = 0; i < _srvResourceBinding.size(); i++) {
				
				auto resource = _binding[_srvResourceBinding[i]];

				
				/*D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Buffer.NumElements = numElements;
				if (elementSize == 0)
				{
					srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;
					srvDesc.Buffer.StructureByteStride = 0;
				}
				else
				{
					srvDesc.Format = DXGI_FORMAT_UNKNOWN;
					srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
					srvDesc.Buffer.StructureByteStride = elementSize;
				}*/
				//UINT descriptorIndex = AllocateDescriptor(&buffer->cpuDescriptorHandle);
				RenderEngineD3D12Impl::Instance()->CreateShaderResourceView(resource->GetResourcePointer());

				

				//tempSRVHandle.ptr += Context::Instance().GetSrvDescriptorSize();
				//RenderEngineD3D12Impl::Instance()->CPUSRVDescriptorHandlePointerMove(1);
			}

			/*bool hasSetUAVGPUHandle = false;
			D3D12_CPU_DESCRIPTOR_HANDLE tempUavDescriptorHandle;

			auto tempUAVHandle = RenderEngineD3D12Impl::Instance()->GetCurrentTopUAVDescriptorHandle();*/

			for (uint i = 0; i < _uavResourceBinding.size(); i++) {
				//auto resource = binding[bd.shaderVarName];
				auto resource = _binding[_uavResourceBinding[i]];


				//auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(backbufferFormat, m_width, m_height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

				
				/*D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
				UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;*/

				RenderEngineD3D12Impl::Instance()->CreateUnorderedAccessView(resource->GetResourcePointer());

				/*device->CreateUnorderedAccessView(resource->GetResourcePointer(), nullptr, nullptr, uavDescriptorHandle);*/

				/*if (!hasSetUAVGPUHandle)
					uavHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_raytracingOutputResourceUAVDescriptorHeapIndex, m_descriptorSize);*/


				//hasSetUAVGPUHandle = true;

				//tempUAVHandle.ptr += Context::Instance().GetUavDescriptorSize();
				//RenderEngineD3D12Impl::Instance()->CPUUAVDescriptorHandlePointerMove(1);
			}
		}
		
		
		// 检查state是否为target state 否则转换
		// 
		void CheckResourceState() {
			for (auto& shaderVarName : _srvResourceBinding) {
				if (_binding[shaderVarName]->GetCurrentState() != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) {
					_binding[shaderVarName]->SetState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
					//D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE for compute pipeline
				}
			}
			for (auto& shaderVarName : _uavResourceBinding) {
				if (_binding[shaderVarName]->GetCurrentState() != D3D12_RESOURCE_STATE_UNORDERED_ACCESS) {
					_binding[shaderVarName]->SetState(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
				}
			}
		}
		
		
		D3D12_GPU_DESCRIPTOR_HANDLE GetSRVBindingHeadDescriptor() {
			return srvHandle;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetUAVBindingHeadDescriptor() {
			return uavHandle;
		}
		string GetSRVTableName() {
			return _srvTableName;
		}

		string GetUAVTableName() {
			return _uavTableName;

		}

	};
}
