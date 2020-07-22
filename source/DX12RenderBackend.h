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
		std::vector<> VertexBuffer;
		std::vector<> PSOInfo;

		// ����
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


