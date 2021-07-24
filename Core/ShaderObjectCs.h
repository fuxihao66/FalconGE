#pragma once
#include "ShaderObject.h"

// TODO: 需要添加lib库；并且dxc需要重新下（sdk的版本太低）
namespace Falcon {
    class ShaderObjectCs: public ShaderObject {
    private:
        void* _csBufferPointer;
        uint _csBufferSize;
    public:
        
        ShaderObjectCs() = default;

        // TODO:  出现数组的时候怎么办，bind range？
        ShaderObjectCs(const std::wstring& csFileName, const std::string& shaderName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st) {


            _st = st;
            //auto pTarget = GetPTarget(shaderType, sm);

            _srvResourceBinding.resize(10000);
            _uavResourceBinding.resize(10000);
            _cbvResourceBinding.resize(10000);

            if (st == ShaderType::RayTracing) {
                // error
            }
            else if (sm >= ShaderModel::SHADER_MODEL_6_0) {
                auto pTarget = GetPTarget(L"cs", sm);
                _csBufferPointer = CreateFromFileDXC(csFileName, entryPoint, pTarget, _csBufferSize);
                

            }
            else {
                auto pTarget = GetPTarget(L"cs", sm);
                _csBufferPointer = CreateFromFileFXC(csFileName, entryPoint, pTarget, _csBufferSize);

            }


            _srvResourceBinding.resize(srvCount);
            _uavResourceBinding.resize(uavCount);
            _cbvResourceBinding.resize(cbvCount);

            _shaderName = shaderName;
            _srvTableName = _shaderName + "_srvTable";
            _uavTableName = _shaderName + "_uavTable";


            // RenderEngineD3D12Impl::Instance()->CreateComputePipeline( _csBufferSize, _csBufferPointer, _shaderName);
        }


        void CreatePipelineStateObject(){
            RenderEngineD3D12Impl::Instance()->CreateComputePipeline( _csBufferSize, _csBufferPointer, _shaderName);

        }
        

    };
}
