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
			break;
		case ApiType::Nan:
			// Å×³öÒì³£
			break;
		default:
			break;
		}
	}
	return *_selfPointer;
}

void RenderBackend::Render() {
	DoRender();
}
void RenderBackend::Dispatch() {
	DoDispatch();
}