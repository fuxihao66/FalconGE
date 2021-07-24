#pragma once

// 启动应用窗口

// 处理消息

// 创建渲染pass





// https://github.com/glfw/glfw/issues/1755
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <iostream>

// d3d12
#include <DirectXMath.h> // For XMVector, XMFLOAT3, XMFLOAT4
#include <comdef.h>      // for _com_error
#include <d3d12.h>       // for D3D12 interface
#include <dxgi1_6.h>     // for DXGI
#include <wrl.h>         // for Microsoft::WRL::ComPtr



#include "RenderEngine.h"



// void process_keystrokes_input(GLFWwindow *window) {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, 1);

//     Falcon::InputManager::SetKeyDown();
//     Falcon::InputManager::SetKeyUp();
// }
void OnResizeFrame(GLFWwindow * window, int w, int h) {
    //Falcon::RenderEngine::ResizeRenderedBuffers(width, height);
    /*width = w;
    height = h;*/

    Falcon::RenderEngineD3D12Impl::Instance()->Resize(w, h);
}
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
    // 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
    // 关闭应用程序
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    //Falcon::InputManager::SetKeyDown();
    //Falcon::InputManager::SetKeyUp();
}


class AppFramework {
protected:
    std::map<std::string, Falcon::RenderPass::Ptr> _passMap;

    virtual void Initialize() {
        for (auto pass : _passMap) {
            pass.second->Initialize(); // shader编译 资源绑定等操作
        }

        // 创建root signature和pipeline state
        Falcon::RenderEngineD3D12Impl::Instance()->ReserveRootSignature(_passMap.size());
        for (auto pass : _passMap) {
            pass.second->RenderPassBegin();// 绑定root signature index
            Falcon::RenderEngineD3D12Impl::Instance()->BuildRootSignature(pass.second->GetShaderObjs()); // 构建root signature

            // 创建完root signature之后再创建管线
            Falcon::RenderEngineD3D12Impl::Instance()->CreatePipelineState(pass.second->GetShaderObjs());

            pass.second->RenderPassEnd();

        }

    }
    int width, height;



public:
    AppFramework() : width(1280), height(720) {
        Initialize();
    }
    AppFramework(int w, int h) : width(w), height(h) {
        Initialize();
    }

    virtual void ExecuteImpl() {}
    void Execute() {
        Falcon::RenderEngineD3D12Impl::Instance()->BeginRecording();
        ExecuteImpl();
        Falcon::RenderEngineD3D12Impl::Instance()->EndRecording();

    }
    int Run() {


        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        auto window =
            glfwCreateWindow(width, height, "glfw - DX12", nullptr, nullptr);
        if (!window) {
            std::cout << "Creat window error" << std::endl;
            return -1;
        }
        glfwSetFramebufferSizeCallback(window, OnResizeFrame);

        glfwSetKeyCallback(window, key_callback);

        auto hMainWnd = glfwGetWin32Window(window);

        //   InitDirect3D(hMainWnd, 800, 600);
        Falcon::RenderEngineD3D12Impl::Instance()->Initialize(hMainWnd, width, height);


        // Resize window for the first time
        OnResizeFrame(window, width, height);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            // process_keystrokes_input(window);

            Execute();


            Falcon::RenderEngineD3D12Impl::Instance()->Present();
        }

        Falcon::RenderEngineD3D12Impl::Instance()->FreeResources();
        glfwTerminate();

        return 0;
    }
};

