#pragma once

#include <string>

#include "../Utility/utility.h"
#include "../Core/ShaderObjectVsPs.h"
namespace Falcon {

    class BaseGraphicsPass {
        // vertex buffer

    protected:
        const std::string _passName;
        ShaderObject::Ptr _shaderObj;
    public:
        // shaderType : "ps"  "vs" "gs" "cs" "lib"
        BaseGraphicsPass(const std::wstring& vsFileName, const std::wstring& psFileName, const std::string& passName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st, uint numRenderTarget) : _passName(passName){
            _shaderObj = std::make_shared<ShaderObjectVsPs>(vsFileName, psFileName,  passName, entryPoint, sm, st, numRenderTarget);
            //Initialize();
        }

        virtual void Initialize() {

        }
        void Begin() {
            RenderEngineD3D12Impl::Instance()->SetPipeline(_passName);
        }

        void BindConstantBuffer(const string& varName, ResourceD3D12Impl::Ptr resource) {
            if (_shaderObj->GetType() == ShaderType::Graphics) {
                RenderEngineD3D12Impl::Instance()->BindConstantToGraphicsPipeline(varName, resource);
            }
            else {
                RenderEngineD3D12Impl::Instance()->BindConstantToComputePipeline(varName, resource);
            }
        }
        void BindShaderResourceBinding(ShaderResourceBindingD3D12Impl::Ptr srb) {
            if (_shaderObj->GetType() == ShaderType::Graphics) {
                RenderEngineD3D12Impl::Instance()->BindResourceBindingToGraphicsPipeline(srb);
            }
            else {
                RenderEngineD3D12Impl::Instance()->BindResourceBindingToComputePipeline(srb);
            }
        }

        ShaderObject::Ptr GetShaderObject() {
            return _shaderObj;
        }

        /*virtual void BindCBVResource(matrix, 0) {

        }*/

        //void BindResourceToBinding(std::map<std::string, Resource::Ptr>& bmap) {
        //    // 创建descriptor


        //}

        //void BindPass() {
        //    // 确定用这个shader

        //    // 
        //    //BindResourceBindingToPipeline(srb);
        //}
        //void UnBindPass() {

        //}

        /*std::string GetBindingName() {
            return srb->GetName();
        }*/

        //virtual void Execute() = 0;
    };
}