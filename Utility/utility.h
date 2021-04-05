// d3d12
#include <DirectXMath.h> // For XMVector, XMFLOAT3, XMFLOAT4
#include <comdef.h>      // for _com_error
#include <d3d12.h>       // for D3D12 interface
#include <dxgi1_6.h>     // for DXGI
#include <wrl.h>         // for Microsoft::WRL::ComPtr
#include <vector>
#include <map>
#include <memory>
#include "d3dx12.h"
namespace Falcon {

    using uint = unsigned int;
    enum ApiType {
        D3D12,
        Vulkan,
        OGL
    };
    /*enum ResourceUsageFlag {
        NONE = 0,
        CBV = 1,
        SRV = 2,
        UAV = 4
    };*/
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


    class FSVertex {
        DirectX::XMFLOAT3 Pos;
        DirectX::XMFLOAT2 UV;
    public:
        FSVertex() {}
        FSVertex(float a, float b, float c, float d, float e) : Pos(a,b,c), UV(d,e)
        {

        }
    };
}