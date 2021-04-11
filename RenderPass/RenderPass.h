#pragma once
#include "../Core/RenderEngine.h"
namespace Falcon{
    class RenderPass{
    public:
        RenderPass(){
            GenerateRootSignatureID();
            Initialize();
        }

        using Ptr = std::shared_ptr<RenderPass>;
        virtual void SetScene(Scene::Ptr pScene) = 0;

        virtual void Execute() = 0;

        uint GetResourceBindingID(){
            return _rsId;
        }

        virtual vector<ShaderObject::Ptr> GetShaderObjs() = 0;


        virtual void Initialize() = 0;


    private:
        uint _rsId;
        void GenerateRootSignatureID(){
            _rsId = RenderEngineD3D12Impl::Instance()->GeneratePossibleRSIndex();
        }

    protected:
        void RenderPassBegin() {
            RenderEngineD3D12Impl::Instance()->SetBindingID(_rsId);
        }

        void RenderPassEnd() {
            RenderEngineD3D12Impl::Instance()->SetBindingID(-1);
        }
    }; 
}
