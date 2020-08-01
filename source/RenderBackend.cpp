#include "RenderBackend.h"


using namespace Falcon;

RenderBackend& RenderBackend::Instance() {
	if (_selfPointer == nullptr) {
		switch (GameContext::Instance().GetApi())
		{
		case ApiType::D3D12:
			_selfPointer = new DX12RenderBackend();
			break;
		case ApiType::D3D11:
			// �׳��쳣
			break;
		case ApiType::OGL:
			// �׳��쳣
			break;
		case ApiType::Vulkan:
			// �׳��쳣
			break;
		case ApiType::Nan:
			// �׳��쳣
			break;
		default:
			break;
		}
	}
	return *_selfPointer;
}

void RenderBackend::Render() {
	switch (GameContext::Instance().GetRenderType()) {
	case RenderType::Deferred:
		DoRenderDeferred();
	case RenderType::ForwardPlus:
		DoRenderForwardPlus();
	}
}
void RenderBackend::Dispatch() {
	DoDispatch();
}