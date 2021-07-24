#pragma once
#include <string>

#include "BaseGraphicsPass.h"
#include "../Core/RenderEngine.h"

enum TOPOLOGY_TYPE{
    TRIANGLE = 0,
    TRIANGLE_STRIP = 1,
    POINT = 2
};

namespace Falcon {

    class ScenePass {
        // vertex buffer

    protected:
        const std::string _passName;
        ShaderObject::Ptr _shaderObj;
    public:
        // shaderType : "ps"  "vs" "gs" "cs" "lib"
        ScenePass(const std::wstring& vsFileName, const std::wstring& psFileName, const std::string& passName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st, uint numRenderTarget) : _passName(passName){
            _shaderObj = std::make_shared<ShaderObjectVsPs>(vsFileName, psFileName,  passName, entryPoint, sm, st, numRenderTarget);
            //Initialize();
        }

        virtual void Initialize() {

        }
        void Begin() {
            RenderEngineD3D12Impl::Instance()->SetPipeline(_passName);
        }

        
    };
}