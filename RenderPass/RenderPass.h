
#include "../Core/RenderEngine.h"
namespace Falcon{
    class RenderPass{
    public:
        RenderPass(){
            GenerateRootSignatureID();
        }
        virtual void SetScene(auto pScene) = 0;


        uint GetResourceBindingID(){
            return _rsId;
        }
    private:
        uint _rsId;
        void GenerateRootSignatureID(){
            _rsId = RenderEngine::Instance()->GeneratePossibleRSIndex();
        }

    protected:
        void Begin() {
            RenderEngine::Instance()->SetBindingID(_rsId);
        }

        void End() {
            RenderEngine::Instance()->SetBindingID(-1);
        }
    }; 
}
