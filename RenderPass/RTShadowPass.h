#include "RenderPass.h"

class RTShadowPass: public RenderPass{


    Falcon::RTPass::SharedPtr shadowPass;

    RTShadowPass(){
        InitializeShader();
    }

    void InitializeShader(){
        
    }
    void execute(){

    }
};