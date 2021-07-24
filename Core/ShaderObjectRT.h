#pragma once
#include "ShaderObject.h"

// TODO: 需要添加lib库；并且dxc需要重新下（sdk的版本太低）
namespace Falcon {

    struct HitTriplet{
        std::string _closestHit;
        std::string _anyHit;
        std::string _intersection;
        HitTriplet(const string& c, const string& a, const string& i){
            _closestHit = c;
            _anyHit = a;
            _intersection = i;
        }
    };

    class ShaderObjectRT: public ShaderObject {
    private:
        void* _csBufferPointer;
        uint _csBufferSize;
        uint _maxRecursion;

        std::string _rayGenName;
        std::map<uint, HitTriplet> _hitGroups;
        std::map<uint, std::string> _miss;
    public:
        
        ShaderObjectRT() = default;

        // TODO:  出现数组的时候怎么办，bind range？
        ShaderObjectRT(const std::wstring& rtFileName, const std::string& shaderName, ShaderModel sm, ShaderType st, uint maxRecursion) {


            _st = st;
            //auto pTarget = GetPTarget(shaderType, sm);

            _srvResourceBinding.resize(10000);
            _uavResourceBinding.resize(10000);
            _cbvResourceBinding.resize(10000);

            if (sm >= ShaderModel::SHADER_MODEL_6_3) {
                auto pTarget = GetPTarget(L"lib", sm);
                _csBufferPointer = CreateFromFileDXR(rtFileName, pTarget, _csBufferSize);
                

            }
            else {
                //error
            }


            _srvResourceBinding.resize(srvCount);
            _uavResourceBinding.resize(uavCount);
            _cbvResourceBinding.resize(cbvCount);

            _shaderName = shaderName;
            _srvTableName = _shaderName + "_srvTable";
            _uavTableName = _shaderName + "_uavTable";

            _maxRecursion = maxRecursion;
            
        }
        void SetDispatchDesc(D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE& HitGroupTable, D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE& MissShaderTable, D3D12_GPU_VIRTUAL_ADDRESS_RANGE_AND_STRIDE& RayGenerationShaderRecord ){
            HitGroupTable.StartAddress = hitGroupShaderTable->GetGPUVirtualAddress();
            HitGroupTable.SizeInBytes = hitGroupShaderTable->GetDesc().Width;
            HitGroupTable.StrideInBytes = hitGroupShaderTableStrideInBytes;
            MissShaderTable.StartAddress = missShaderTable->GetGPUVirtualAddress();
            MissShaderTable.SizeInBytes = missShaderTable->GetDesc().Width;
            MissShaderTable.StrideInBytes = missShaderTableStrideInBytes;
            RayGenerationShaderRecord.StartAddress = rayGenShaderTable->GetGPUVirtualAddress();
            RayGenerationShaderRecord.SizeInBytes = rayGenShaderTable->GetDesc().Width;
        }
        

        void SetRayGen(const string& rayGenName){
            _rayGenName = rayGenName;
        }

        void AddHitGroup(uint hitIndex, const std::string& closestHit, const std::string& anyHit = "", const std::string& intersection = ""){
            _hitGroups[hitIndex] = HitTriplet(closestHit, anyHit, intersection);
        }
        void AddMiss(uint missIndex, const std::string& missName){
            _miss[missIndex] = missName;
        }

        void CreatePipelineStateObject(){
            RenderEngineD3D12Impl::Instance()->CreateRTPipeline( _csBufferSize, _csBufferPointer, _shaderName, _maxRecursion);
        }
        

    };
}
