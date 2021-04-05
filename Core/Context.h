/*
Api相关资源
*/
#include "../Utility/utility.h"
namespace Falcon {
	class Context {

	private:
		ApiType _api;
		static Context* _selfPointer;
		Context() {

		}
	public:
		static Context Instance() {
			if (_selfPointer == nullptr) {
				_selfPointer = new Context();
			}
			return *_selfPointer;
		}

		ApiType GetApi() {
			return _api;
		}
		GetD3D12Device() {

		}

		GetD3D12CommandList() {

		}
	};
}
