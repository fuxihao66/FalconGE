#pragma once

// d3d12
#include <DirectXMath.h> // For XMVector, XMFLOAT3, XMFLOAT4
#include <comdef.h>      // for _com_error
#include <d3d12.h>       // for D3D12 interface
#include <dxgi1_6.h>     // for DXGI
#include <wrl.h>         // for Microsoft::WRL::ComPtr
#include <vector>
#include <map>
#include <memory>
#include <d3d12shader.h>
#include <d3dcompiler.h>
#include <atlbase.h>        // Common COM helpers.
#include "dxcapi.h"  
#include "d3dx12.h"


inline void DxTrace(const wchar_t* file, unsigned long line, HRESULT hr,
    const wchar_t* proc) {
    _com_error err(hr);
    std::cerr << "file:" << file << "line:" << line << ", " << proc
        << " Error: " << (const char*)err.Description() << std::endl;
}

#define V_RETURN(op)                                                           \
  if (FAILED(hr = (op))) {                                                     \
    assert(0);                                                                 \
    DxTrace(__FILEW__, __LINE__, hr, L#op);                                    \
    return hr;                                                                 \
  }

#define V(op)                                                                  \
  if (FAILED(hr = (op))) {                                                     \
    assert(0);                                                                 \
    DxTrace(__FILEW__, __LINE__, hr, L#op);                                    \
  }

#define SAFE_ADDREF(obj) ((obj) ? (obj)->AddRef() : 0)
#define SAFE_RELEASE(obj) ((obj) ? (obj)->Release() : 0)

using Microsoft::WRL::ComPtr;
using namespace DirectX;
namespace Falcon {

    inline Scene::Ptr LoadScene(const string& sceneFile) {

    }


    enum TOPOLOGY_TYPE {
        TRIANGLE = 0,
        TRIANGLE_STRIP = 1,
        POINT = 2
    };
    enum ShaderType {
        Graphics,
        Compute,
        RayTracing
    };

    enum ShaderModel {
        SHADER_MODEL_1,
        SHADER_MODEL_2,
        SHADER_MODEL_3,
        SHADER_MODEL_4,
        SHADER_MODEL_5,
        SHADER_MODEL_5_1,
        SHADER_MODEL_6_0,
        SHADER_MODEL_6_1,
        SHADER_MODEL_6_2,
        SHADER_MODEL_6_3,
        SHADER_MODEL_6_4,
        SHADER_MODEL_6_5
    };
    using uint = unsigned int;
    using uint2 = DirectX::XMUINT2;
    enum ApiType {
        D3D12,
        Vulkan,
        OGL
    };
    struct BindingDesc {
        std::string shaderVarName;
        int registerIndex;
        int registerSpace;
    };
    enum ResourceUsageFlag {
        NONE = 0,
        CBV = 1,
        SRV = 2,
        UAV = 4
    };
    enum ResourceState {
        STATE_COMMON = 0,
        STATE_VERTEX_AND_CONSTANT_BUFFER = 0x1,
        STATE_INDEX_BUFFER = 0x2,
        STATE_RENDER_TARGET = 0x4,
        STATE_UNORDERED_ACCESS = 0x8,
        STATE_DEPTH_WRITE = 0x10,
        STATE_DEPTH_READ = 0x20,
        STATE_NON_PIXEL_SHADER_RESOURCE = 0x40,
        STATE_PIXEL_SHADER_RESOURCE = 0x80,
        STATE_STREAM_OUT = 0x100,
        STATE_INDIRECT_ARGUMENT = 0x200,
        STATE_COPY_DEST = 0x400,
        STATE_COPY_SOURCE = 0x800,
        STATE_RESOLVE_DEST = 0x1000,
        STATE_RESOLVE_SOURCE = 0x2000,
        STATE_RAYTRACING_ACCELERATION_STRUCTURE = 0x400000,
        STATE_SHADING_RATE_SOURCE = 0x1000000,
        STATE_GENERIC_READ = (((((0x1 | 0x2) | 0x40) | 0x80) | 0x200) | 0x800),
        STATE_PRESENT = 0,
        STATE_PREDICATION = 0x200,
        STATE_VIDEO_DECODE_READ = 0x10000,
        STATE_VIDEO_DECODE_WRITE = 0x20000,
        STATE_VIDEO_PROCESS_READ = 0x40000,
        STATE_VIDEO_PROCESS_WRITE = 0x80000,
        STATE_VIDEO_ENCODE_READ = 0x200000,
        STATE_VIDEO_ENCODE_WRITE = 0x800000

    };


    inline std::string WStr2Str(LPCWSTR wstr) {
        int nLen = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);

        if (nLen <= 0) return std::string("");

        char* pszDst = new char[nLen];
        if (NULL == pszDst) return std::string("");

        WideCharToMultiByte(CP_ACP, 0, wstr, -1, pszDst, nLen, NULL, NULL);
        pszDst[nLen - 1] = 0;

        std::string strTemp(pszDst);
        delete[] pszDst;

        return strTemp;
    }



    //shaderType cs ps lib vs gs
    inline std::wstring GetPTarget(const std::wstring& shaderType, ShaderModel sm) {
        auto pTarget = shaderType;

        switch (sm) {
        case SHADER_MODEL_1:
            pTarget += L"_1_1";
            break;
        case SHADER_MODEL_2:
            pTarget += L"_2_0";
            break;
        case SHADER_MODEL_3:
            pTarget += L"_3_0";
            break;
        case SHADER_MODEL_4:
            pTarget += L"_4_1";
            break;
        case SHADER_MODEL_5:
            pTarget += L"_5_0";
            break;
        case SHADER_MODEL_5_1:
            pTarget += L"_5_0";
            break;
        default:
            pTarget += L"_6_0";
            break;
        }

        return pTarget;
    }

    class FSVertex {
        DirectX::XMFLOAT3 Pos;
        DirectX::XMFLOAT2 UV;
    public:
        FSVertex() = default;
        FSVertex(float a, float b, float c, float d, float e) : Pos(a,b,c), UV(d,e)
        {

        }
    };
}