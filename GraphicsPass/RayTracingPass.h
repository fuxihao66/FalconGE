#pragma once

#include <string>

#include "../Utility/utility.h"
#include "../Core/ShaderObjectRT.h"
namespace Falcon {

    class RayTracingPass {
        // vertex buffer

    protected:
        const std::string _passName;
        ShaderObjectRT::Ptr _shaderObj;
    public:
        // shaderType : "ps"  "vs" "gs" "cs" "lib"
        RayTracingPass(const std::wstring& rtFileName,  const std::string& passName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st, uint maxRecursion) : _passName(passName){
            _shaderObj = std::make_shared<ShaderObjectRT>(rtFileName, passName, sm, st, maxRecursion);
            //Initialize();
        }

        virtual void Initialize() {

        }
        void Begin() {
            RenderEngineD3D12Impl::Instance()->SetRTPipeline(_passName);
        }

        void Execute(uint3 dispatchDim){

            D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};

            _shaderObj->SetDispatchDesc(dispatchDesc.HitGroupTable, dispatchDesc.MissShaderTable, dispatchDesc.RayGenerationShaderRecord);
            
            dispatchDesc.Width = dispatchDim.x;
            dispatchDesc.Height = dispatchDim.y;
            dispatchDesc.Depth = dispatchDim.z;


            RenderEngineD3D12Impl::Instance()->DispatchRayTrace(&dispatchDesc);

        }
        
    };
}