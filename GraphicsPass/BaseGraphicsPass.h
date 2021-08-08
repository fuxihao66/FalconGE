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
        BaseGraphicsPass(const string& passName) : _passName(passName){
            //Initialize();
        }

        virtual void Initialize() {

        }
        virtual void Begin() {
            // RenderEngineD3D12Impl::Instance()->SetPipeline(_passName);
        }

        void BindAccelerationStruct(ResourceD3D12Impl::Ptr resource){
            RenderEngineD3D12Impl::Instance()->BindAccelerationStructToPipeline("gScene_"+_shaderObj->GetShaderName(), resource);

        }

        void BindConstantBuffer(const string& varName, ResourceD3D12Impl::Ptr resource) {
            if (_shaderObj->GetType() == ShaderType::Graphics) {
                RenderEngineD3D12Impl::Instance()->BindConstantToGraphicsPipeline(varName+"_"+_shaderObj->GetShaderName(), resource);
            }
            else {
                RenderEngineD3D12Impl::Instance()->BindConstantToComputePipeline(varName+"_"+_shaderObj->GetShaderName(),resource);
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


        //}

        //void BindPass() {

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