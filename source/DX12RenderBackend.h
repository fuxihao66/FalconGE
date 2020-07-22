#pragma once

#include "RenderBackend.h"

// TODO:存储了每个物体的vertex表示和pso信息
// 包括了指向这些的指针
// 

namespace Falcon {

	// 具体的渲染引擎
	class DX12RenderBackend : public RenderBackend
	{
	private:
		std::vector<> VertexBuffer;
		std::vector<> PSOInfo;

		// 索引
		std::vector<RenderItem> RenderQueue;
		/*for (auto obj : RenderQueue) {

		}*/
	public:
		virtual void InitializePipeline();    // initialize global context
		virtual void InitializeResources();          // initialize resources


		// virtual methods   
		virtual void DoRender();
		virtual void DoDispatch();
	};
}


