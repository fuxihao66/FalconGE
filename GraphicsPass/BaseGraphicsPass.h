

#include <string>

#include "../Utility/utility.h"

namespace Falcon {

    class BaseGraphicsPass {
        // vertex buffer

    private:


        //static BaseGraphicsPass::Ptr Create(const std::string& path);
    public:
        virtual void Begin() {
            Falcon::RenderEngine::SetPipeline(name);
        }
        /*virtual void BindCBVResource(matrix, 0) {

        }*/

        //void BindResourceToBinding(std::map<std::string, Resource::Ptr>& bmap) {
        //    // ����descriptor


        //}

        //void BindPass() {
        //    // ȷ�������shader

        //    // 
        //    //BindResourceBindingToPipeline(srb);
        //}
        //void UnBindPass() {

        //}

        /*std::string GetBindingName() {
            return srb->GetName();
        }*/

        virtual void Execute() = 0;
    };
}