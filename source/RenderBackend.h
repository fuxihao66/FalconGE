#pragma once
/*
 * ��Ⱦ���  apiϸ�� 
 * �ö���洢�˵�ǰ������api��ʾ
*/
#include "Global.h"
#include "DX12RenderBackend.h"

// ȫ��ʵ��
namespace Falcon{
    class RenderBackend
    {
    private:
        static RenderBackend* _selfPointer;
    public:
        static RenderBackend& Instance();
        virtual void InitializePipeline() = 0;
        virtual void InitializeResources() = 0;

        void Render();
        void Dispatch(); // ����compute shader������


        // �����backend����ʵ��
        virtual void DoRender() = 0;
        virtual void DoDispatch() = 0;

        virtual void ToAPIRealtedResource(std::shared_ptr<Renderable> objPtr);
    };
}


