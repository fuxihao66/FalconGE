#pragma once

#include <string>

#include "../Utility/utility.h"
#include "../Core/ShaderObjectCs.h"
namespace Falcon {

    class ComputePass {
        // vertex buffer

    protected:
        const std::string _passName;
        ShaderObject::Ptr _shaderObj;
    public:
        // shaderType : "ps"  "vs" "gs" "cs" "lib"
        ComputePass(const std::wstring& csFileName,  const std::string& passName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st) : _passName(passName){
            _shaderObj = std::make_shared<ShaderObjectCs>(csFileName, passName, entryPoint, sm, st);
            //Initialize();
        }

        virtual void Initialize() {

        }
        void Begin() {
            RenderEngineD3D12Impl::Instance()->SetRTPipeline(_passName);
        }

        void Execute(){

        }
        
    };
}