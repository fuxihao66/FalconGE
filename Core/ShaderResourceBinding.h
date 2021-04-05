
#include "../Utility/utility.h"
using std::shared_ptr;
using std::vector;
using std::map;
using std::string;

struct BindingDesc {
	std::string shaderVarName;
	int registerIndex;
	int registerSpace;
};
class ShaderResourceBinding {

};
class ShaderResourceBindingD3D12Impl : public ShaderResourceBinding {

	const std::string _srvTableName;
	const std::string _uavTableName;


	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE uavHandle;
public:
	using Ptr = shared_ptr<ShaderResourceBindingD3D12Impl>;
	/*D3D12_GPU_DESCRIPTOR_HANDLE GetSRVBindingHeadDescriptor() {
		return srvHandle;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE GetUAVBindingHeadDescriptor() {
		return uavHandle;
	}*/

	void Bind() {

	}

	void Create(vector<BindingDesc>& srvResource, vector<BindingDesc>& uavResource, map<string, Resource::Ptr> binding) {

		

		// ´´½¨descriptor
		srvHandle = Context::Instance()->GetCurrentTopSRVDescriptorHandle();
		for (auto& bd : srvResource) {
			auto resource = binding[bd.shaderVarName];


			D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
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
			}
			UINT descriptorIndex = AllocateDescriptor(&buffer->cpuDescriptorHandle);
			device->CreateShaderResourceView(buffer->resource.Get(), &srvDesc, buffer->cpuDescriptorHandle);
			buffer->gpuDescriptorHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, m_descriptorSize);
		}


		uavHandle = RenderEngine::Instance()->GetCurrentTopUAVDescriptorHandle();

		for (auto& bd : uavResource) {
			auto resource = binding[bd.shaderVarName];



			auto uavDesc = CD3DX12_RESOURCE_DESC::Tex2D(backbufferFormat, m_width, m_height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

			auto defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			ThrowIfFailed(device->CreateCommittedResource(
				&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &uavDesc, D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&m_raytracingOutput)));
			NAME_D3D12_OBJECT(m_raytracingOutput);

			D3D12_CPU_DESCRIPTOR_HANDLE uavDescriptorHandle;
			m_raytracingOutputResourceUAVDescriptorHeapIndex = AllocateDescriptor(&uavDescriptorHandle, m_raytracingOutputResourceUAVDescriptorHeapIndex);
			D3D12_UNORDERED_ACCESS_VIEW_DESC UAVDesc = {};
			UAVDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
			device->CreateUnorderedAccessView(m_raytracingOutput.Get(), nullptr, &UAVDesc, uavDescriptorHandle);
			m_raytracingOutputResourceUAVGpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_raytracingOutputResourceUAVDescriptorHeapIndex, m_descriptorSize);
		}
	}
	string GetSRVTableName() {

	}

	string GetUAVTableName() {

	}

};