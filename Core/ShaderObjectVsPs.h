#pragma once
#include "ShaderObject.h"

// TODO: 需要添加lib库；并且dxc需要重新下（sdk的版本太低）
namespace Falcon {
    class ShaderObjectVsPs: public ShaderObject {
    private:
        void* _vsBufferPointer;
        void* _psBufferPointer;
        void* _gsBufferPointer;
        uint _vsBufferSize;
        uint _psBufferSize;
        uint _gsBufferSize;
        uint _numRenderTarget;
    public:
        
        ShaderObjectVsPs() = default;

        // TODO:  出现数组的时候怎么办，bind range？
        ShaderObjectVsPs(const std::wstring& vsFileName, const std::wstring& psFileName, const std::string& shaderName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st, uint numRenderTarget) {


            _st = st;
            //auto pTarget = GetPTarget(shaderType, sm);

            _srvResourceBinding.resize(10000);
            _uavResourceBinding.resize(10000);
            _cbvResourceBinding.resize(10000);

            if (st == ShaderType::RayTracing) {
                // error
            }
            else if (sm >= ShaderModel::SHADER_MODEL_6_0) {
                auto pTarget = GetPTarget(L"vs", sm);
                _vsBufferPointer = CreateFromFileDXC(vsFileName, entryPoint, pTarget, _vsBufferSize);
                pTarget = GetPTarget(L"ps", sm);
                _psBufferPointer = CreateFromFileDXC(psFileName, entryPoint, pTarget, _psBufferSize);

            }
            else {
                auto pTarget = GetPTarget(L"vs", sm);
                _vsBufferPointer = CreateFromFileFXC(vsFileName, entryPoint, pTarget, _vsBufferSize);
                pTarget = GetPTarget(L"ps", sm);
                _psBufferPointer = CreateFromFileFXC(psFileName, entryPoint, pTarget, _psBufferSize);

            }


            _srvResourceBinding.resize(srvCount);
            _uavResourceBinding.resize(uavCount);
            _cbvResourceBinding.resize(cbvCount);

            _shaderName = shaderName;
            _srvTableName = _shaderName + "_srvTable";
            _uavTableName = _shaderName + "_uavTable";


            // RenderEngineD3D12Impl::Instance()->CreateGraphicsPipeline(_shaderName, _vsBufferSize, _vsBufferPointer, _psBufferSize, _psBufferPointer, numRenderTarget);
            _numRenderTarget = numRenderTarget;
        }


        
        void CreatePipelineStateObject(){
            RenderEngineD3D12Impl::Instance()->CreateGraphicsPipeline(_shaderName, _vsBufferSize, _vsBufferPointer, _psBufferSize, _psBufferPointer, _numRenderTarget);

        }

    };
}
