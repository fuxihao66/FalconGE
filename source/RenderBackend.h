#pragma once
/*
 * ��Ⱦ���  apiϸ�� 
 * �ö���洢�˵�ǰ������api��ʾ
*/

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>

#include "Global.h"
#include "DX12RenderBackend.h"

using Microsoft::WRL::ComPtr;
//using namespace std;
using namespace DirectX;

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")
// ȫ��ʵ��
namespace Falcon{
    class RenderBackend
    {
    private:
        static RenderBackend* _selfPointer;
    public:
        static RenderBackend& Instance();
        /*virtual void InitializePipeline() = 0;
        virtual void InitializeResources() = 0;*/

        void Render();
        void Dispatch(); // ����compute shader������


        virtual void AddTex();
        virtual void BulidPSO(std::shared_ptr<Shader> shaderPtr, const std::string& psokey);

        virtual void AddMat();
        virtual void AddObj(std::shared_ptr<Renderable> ptr, const std::string& id);
        // TODO: ��������  �������ڶ��㶯��
        virtual void UpdateObj(std::shared_ptr<Renderable> ptr, const std::string& id);

        virtual void AddRenderTexture();
        virtual void CreatePass();
        virtual void AddAE();


        // �����backend����ʵ��
        virtual void DoRenderDeferred() = 0;
        virtual void DoRenderForwardPlus() = 0;
        virtual void DoDispatch() = 0;

        //virtual void ToAPIRealtedResource(std::shared_ptr<Renderable> objPtr);
    };
}


