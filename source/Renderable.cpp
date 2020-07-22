#include "Renderable.h"
using namespace Falcon;

ErrorType Renderable::AddCallBackUpdate(std::function<void()>& f) {
	UpdateCallback.push_back(f);

	return ErrorType::Success;
}
