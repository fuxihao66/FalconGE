#pragma once
/*
 * 渲染后端  api细节 
 * 该对象存储了当前场景的api表示
*/
#include "Global.h"
#include "DX12RenderBackend.h"

// 全局实例
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
        void Dispatch(); // 用于compute shader，阻塞


        // 具体的backend进行实现
        virtual void DoRender() = 0;
        virtual void DoDispatch() = 0;

        virtual void ToAPIRealtedResource(std::shared_ptr<Renderable> objPtr);
    };
}


