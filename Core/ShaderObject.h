#pragma once
#include "ShaderResourceBinding.h"

// TODO: ��Ҫ����lib�⣻����dxc��Ҫ�����£�sdk�İ汾̫�ͣ�
namespace Falcon {
    class ShaderObject {
    protected:
        string _shaderName;
        vector<std::string> _srvResourceBinding;   // index��register bindpoint
        vector<std::string> _uavResourceBinding;
        vector<std::string> _cbvResourceBinding;


        int _ASBindingPoint = -1;

        string _srvTableName;
        string _uavTableName;
        uint _space = 0;


        int srvCount = 0;
        int uavCount = 0;
        int cbvCount = 0;

        uint _BufferSize;
        void* _BufferPointer;
        ShaderType _st;
    public:
        using Ptr = std::shared_ptr<ShaderObject>;
        ShaderResourceBindingD3D12Impl::Ptr CreateShaderResourceBinding() {
            // ����srb  �û�����Ϊһ��shader�������srb������ͬһ������  ��ͬ���ʣ�

            return std::make_shared<ShaderResourceBindingD3D12Impl>(_srvTableName, _uavTableName, _srvResourceBinding, _uavResourceBinding);
        }

        ShaderType GetType() {
            return _st;
        }


        ShaderObject() = default;

        // TODO:  ���������ʱ����ô�죬bind range��
        ShaderObject(const std::wstring& fileName, const std::string& shaderName, const std::wstring& entryPoint, ShaderModel sm, ShaderType st) {
            _st = st;
            //auto pTarget = GetPTarget(shaderType, sm);

            _srvResourceBinding.resize(10000);
            _uavResourceBinding.resize(10000);
            _cbvResourceBinding.resize(10000);

            if (st == ShaderType::RayTracing) {
                uint bufferSize;
                auto pTarget = GetPTarget(L"lib", sm);
                _BufferPointer = CreateFromFileDXR(fileName, pTarget, _BufferSize);
            }
            else if (sm >= ShaderModel::SHADER_MODEL_6_0) {
                auto pTarget = GetPTarget(L"cs", sm);

                _BufferPointer = CreateFromFileDXC(fileName, entryPoint, pTarget, _BufferSize);
            }
            else {
                auto pTarget = GetPTarget(L"cs", sm);
                _BufferPointer = CreateFromFileFXC(fileName, entryPoint, pTarget, _BufferSize);
            }

            _srvResourceBinding.resize(srvCount);
            _uavResourceBinding.resize(uavCount);
            _cbvResourceBinding.resize(cbvCount);

            _shaderName = shaderName;
            _srvTableName = _shaderName + "_srvTable";
            _uavTableName = _shaderName + "_uavTable";

            if (st == ShaderType::RayTracing) {
                //RenderEngineD3D12Impl::Instance()->CreateRTPipeline(_BufferSize, _BufferPointer, _shaderName);
            }
            else
                RenderEngineD3D12Impl::Instance()->CreateComputePipeline(_BufferSize, _BufferPointer, _shaderName);
        }

        std::string GetShaderName() const{
            return _shaderName;
        }

        std::string GetSRVTableName() const {
            return _srvTableName;
        }
        std::string GetUAVTableName() const {
            return _uavTableName;
        }

        uint GetSRVTableSize() const {
            return _srvResourceBinding.size();
        }

        uint GetUAVTableSize() const {
            return _uavResourceBinding.size();
        }

        const vector<std::string>& GetConstantBinding() const {
            return _cbvResourceBinding;
        }

        int GetASBindPoint()const {
            return _ASBindingPoint;
        }

        uint GetSpace() const {
            return _space;
        }

        template <class T>
        void ReadFromReflection(T resource_desc, int& srvCount, int & uavCount, int& cbvCount) {
            auto shaderVarName = resource_desc.Name;
            auto registerSpace = resource_desc.Space;
            auto resourceType = resource_desc.Type;
            auto registerIndex = resource_desc.BindPoint;


            if (std::string(resource_desc.Name) == "gScene"){
                _ASBindingPoint = registerIndex;
            }
            else if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_TBUFFER ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_STRUCTURED ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_BYTEADDRESS ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_RTACCELERATIONSTRUCTURE) {
                _srvResourceBinding[registerIndex] = shaderVarName;
                srvCount++;

            }
            else if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWTYPED ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWBYTEADDRESS ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_APPEND_STRUCTURED ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_CONSUME_STRUCTURED ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_RWSTRUCTURED_WITH_COUNTER ||
                resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_UAV_FEEDBACKTEXTURE) {
                _uavResourceBinding[registerIndex] = shaderVarName;
                uavCount++;


            }// TODO: 这里存储的不是var name，而是 shaderVarName+"_"+_shaderName  主要是renderEngine中索引slot时候防止不同的shader中有同样变量名的情况
            else if (resourceType == D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER) {
                // _cbvResourceBinding[registerIndex] = shaderVarName;
                _cbvResourceBinding[registerIndex] = shaderVarName+"_"+_shaderName;
                cbvCount++;



            }
            _space = registerSpace;
        }
        
        //shaderType cs ps vs  gs lib
        void* CreateFromFileFXC(const std::wstring& filename, const std::wstring& entryPoint, const std::wstring& pTarget, uint& shaderBufferSize) {
            ID3DBlob* pPSBlob = nullptr;
            ID3DBlob* pErrorBlob = nullptr;

            auto strPTarget = WStr2Str(pTarget.c_str());
            auto strEntryPoint = WStr2Str(entryPoint.c_str());




            D3DCompileFromFile(filename.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint.c_str(), strPTarget.c_str(), 0, 0, &pPSBlob, &pErrorBlob);




            if (pErrorBlob)
            {
                OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
                pErrorBlob->Release();
            }

            ID3D12ShaderReflection* pReflection = NULL;
            D3DReflect(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), IID_ID3D12ShaderReflection, (void**)&pReflection);


            /*std::cout << WStr2Str(filename.c_str()) << std::endl;
            std::cout << WStr2Str(entryPoint.c_str()) << std::endl;
            std::cout << WStr2Str(pTarget.c_str()) << std::endl;*/
            //
            //RenderEngineD3D12Impl::Instance()->CreatePipeline(pPSBlob->GetBufferSize(), pPSBlob->GetBufferPointer(), _shaderName);
            //
            shaderBufferSize = pPSBlob->GetBufferSize();
            void* bufferPointer = pPSBlob->GetBufferPointer();


            D3D12_SHADER_DESC shader_desc;
            pReflection->GetDesc(&shader_desc);


            

            for (int i = 0; i < shader_desc.BoundResources; i++)
            {
                D3D12_SHADER_INPUT_BIND_DESC  resource_desc;
                pReflection->GetResourceBindingDesc(i, &resource_desc);

                ReadFromReflection(resource_desc, srvCount, uavCount, cbvCount);


            }


            

            return bufferPointer;
        }


        // TODO: 
        void* CreateFromFileDXR(const std::wstring& filename, const std::wstring& pTarget, uint& shaderBufferSize) {
            CComPtr<IDxcUtils> pUtils;
            CComPtr<IDxcCompiler3> pCompiler;
            DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
            DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

            //
            // Create default include handler. (You can create your own...)
            //
            CComPtr<IDxcIncludeHandler> pIncludeHandler;
            pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);


            //
            // COMMAND LINE: dxc myshader.hlsl -E main -T ps_6_0 -Zi -D MYDEFINE=1 -Fo myshader.bin -Fd myshader.pdb -Qstrip_reflect
            //
            LPCWSTR pszArgs[] =
            {
                filename.c_str(),            // Optional shader source file name for error reporting and for PIX shader source view.  
                //L"-E", L"main",              // Entry point.
                //L"-T", L"lib_6_3",            // Target.
                L"-T", pTarget.c_str(),            // Target.
                L"-Zi",                      // Enable debug information.
                L"-D", L"MYDEFINE=1",        // A single define.
                L"-Fo", L"myshader.bin",     // Optional. Stored in the pdb. 
                L"-Fd", L"myshader.pdb",     // The file name of the pdb. This must either be supplied or the autogenerated file name must be used.
                L"-Qstrip_reflect",          // Strip reflection into a separate blob. 
            };


            //
            // Open source file.  
            //
            CComPtr<IDxcBlobEncoding> pSource = nullptr;
            pUtils->LoadFile(filename.c_str(), nullptr, &pSource);
            DxcBuffer Source;
            Source.Ptr = pSource->GetBufferPointer();
            Source.Size = pSource->GetBufferSize();
            Source.Encoding = DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.


            //
            // Compile it with specified arguments.
            //
            CComPtr<IDxcResult> pResults;
            pCompiler->Compile(
                &Source,                // Source buffer.
                pszArgs,                // Array of pointers to arguments.
                _countof(pszArgs),      // Number of arguments.
                pIncludeHandler,        // User-provided interface to handle #include directives (optional).
                IID_PPV_ARGS(&pResults) // Compiler output status, buffer, and errors.
            );

            //
            // Print errors if present.
            //
            CComPtr<IDxcBlobUtf8> pErrors = nullptr;
            pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
            // Note that d3dcompiler would return null if no errors or warnings are present.  
            // IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if there are no warnings or errors.
            if (pErrors != nullptr && pErrors->GetStringLength() != 0)
                wprintf(L"Warnings and Errors:\n%S\n", pErrors->GetStringPointer());

            //
            // Quit if the compilation failed.
            //
            HRESULT hrStatus;
            pResults->GetStatus(&hrStatus);
            if (FAILED(hrStatus))
            {
                wprintf(L"Compilation Failed\n");
                return false;
            }

            //
            // Save shader binary.
            //
            CComPtr<IDxcBlob> pShader = nullptr;
            CComPtr<IDxcBlobUtf16> pShaderName = nullptr;
            pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pShaderName);
            void* bufferPointer = nullptr;
            if (pShader != nullptr)
            {
                //RenderEngineD3D12Impl::Instance()->CreateRTPipeline(pShader->GetBufferSize(), pShader->GetBufferPointer(), _shaderName);
                shaderBufferSize = pShader->GetBufferSize();
                bufferPointer = pShader->GetBufferPointer();
            }

            // reflection
            CComPtr< ID3D12LibraryReflection > pReflection;  // reflection


            CComPtr<IDxcBlob> pReflectionData;
            pResults->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr);
            if (pReflectionData != nullptr)
            {
                // Optionally, save reflection blob for later here.

                // Create reflection interface.
                DxcBuffer ReflectionData;
                ReflectionData.Encoding = DXC_CP_ACP;
                ReflectionData.Ptr = pReflectionData->GetBufferPointer();
                ReflectionData.Size = pReflectionData->GetBufferSize();

                pUtils->CreateReflection(&ReflectionData, IID_PPV_ARGS(&pReflection));

                // Use reflection interface here.

            }


            D3D12_LIBRARY_DESC libdesc;

            pReflection->GetDesc(&libdesc);

            

            for (auto i = 0; i < libdesc.FunctionCount; i++) {
                // hit miss raygen
                auto functionReflect = pReflection->GetFunctionByIndex(i);

                D3D12_FUNCTION_DESC  shader_desc;
                functionReflect->GetDesc(&shader_desc);


                for (int i = 0; i < shader_desc.BoundResources; i++)
                {
                    D3D12_SHADER_INPUT_BIND_DESC  resource_desc;
                    functionReflect->GetResourceBindingDesc(i, &resource_desc);


                    
                    ReadFromReflection(resource_desc, srvCount, uavCount, cbvCount);
                }
            }

            

            return bufferPointer;

        }
        void* CreateFromFileDXC(const std::wstring& filename, const std::wstring& entryPoint, const std::wstring& pTarget, uint& shaderBufferSize) {

            CComPtr<IDxcUtils> pUtils;
            CComPtr<IDxcCompiler3> pCompiler;
            DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&pUtils));
            DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&pCompiler));

            //
            // Create default include handler. (You can create your own...)
            //
            CComPtr<IDxcIncludeHandler> pIncludeHandler;
            pUtils->CreateDefaultIncludeHandler(&pIncludeHandler);

            /*std::cout << WStr2Str(filename.c_str()) << std::endl;
            std::cout << WStr2Str(entryPoint.c_str()) << std::endl;
            std::cout << WStr2Str(pTarget.c_str()) << std::endl;*/

            //
            // COMMAND LINE: dxc myshader.hlsl -E main -T ps_6_0 -Zi -D MYDEFINE=1 -Fo myshader.bin -Fd myshader.pdb -Qstrip_reflect
            //
            LPCWSTR pszArgs[] =
            {
                filename.c_str(),            // Optional shader source file name for error reporting and for PIX shader source view.  
                //L"-E", L"main",              // Entry point.
                //L"-T", L"ps_6_0",            // Target.
                L"-E", entryPoint.c_str(),              // Entry point.
                L"-T", pTarget.c_str(),            // Target.
                L"-Zi",                      // Enable debug information.
                L"-D", L"MYDEFINE=1",        // A single define.
                L"-Fo", L"myshader.bin",     // Optional. Stored in the pdb. 
                L"-Fd", L"myshader.pdb",     // The file name of the pdb. This must either be supplied or the autogenerated file name must be used.
                L"-Qstrip_reflect",          // Strip reflection into a separate blob. 
            };


            //
            // Open source file.  
            //
            CComPtr<IDxcBlobEncoding> pSource = nullptr;
            pUtils->LoadFile(filename.c_str(), nullptr, &pSource);
            DxcBuffer Source;
            Source.Ptr = pSource->GetBufferPointer();
            Source.Size = pSource->GetBufferSize();
            Source.Encoding = DXC_CP_ACP; // Assume BOM says UTF8 or UTF16 or this is ANSI text.


            //
            // Compile it with specified arguments.
            //
            CComPtr<IDxcResult> pResults;
            pCompiler->Compile(
                &Source,                // Source buffer.
                pszArgs,                // Array of pointers to arguments.
                _countof(pszArgs),      // Number of arguments.
                pIncludeHandler,        // User-provided interface to handle #include directives (optional).
                IID_PPV_ARGS(&pResults) // Compiler output status, buffer, and errors.
            );

            //
            // Print errors if present.
            //
            CComPtr<IDxcBlobUtf8> pErrors = nullptr;
            pResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&pErrors), nullptr);
            // Note that d3dcompiler would return null if no errors or warnings are present.  
            // IDxcCompiler3::Compile will always return an error buffer, but its length will be zero if there are no warnings or errors.
            if (pErrors != nullptr && pErrors->GetStringLength() != 0)
                wprintf(L"Warnings and Errors:\n%S\n", pErrors->GetStringPointer());

            //
            // Quit if the compilation failed.
            //
            HRESULT hrStatus;
            pResults->GetStatus(&hrStatus);
            if (FAILED(hrStatus))
            {
                wprintf(L"Compilation Failed\n");
                return false;
            }

            //
            // Save shader binary.
            //
            CComPtr<IDxcBlob> pShader = nullptr;
            CComPtr<IDxcBlobUtf16> pShaderName = nullptr;
            pResults->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&pShader), &pShaderName);

            void* bufferPointer = nullptr;

            if (pShader != nullptr)
            {
                //RenderEngineD3D12Impl::Instance()->CreateComputePipeline(pShader->GetBufferSize(), pShader->GetBufferPointer(), _shaderName);
                shaderBufferSize = pShader->GetBufferSize();
                bufferPointer = pShader->GetBufferPointer();
            }

            CComPtr< ID3D12ShaderReflection > pReflection;

            CComPtr<IDxcBlob> pReflectionData;
            pResults->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&pReflectionData), nullptr);
            if (pReflectionData != nullptr)
            {
                // Optionally, save reflection blob for later here.

                // Create reflection interface.
                DxcBuffer ReflectionData;
                ReflectionData.Encoding = DXC_CP_ACP;
                ReflectionData.Ptr = pReflectionData->GetBufferPointer();
                ReflectionData.Size = pReflectionData->GetBufferSize();

                pUtils->CreateReflection(&ReflectionData, IID_PPV_ARGS(&pReflection));

                // Use reflection interface here.

            }

            
            D3D12_SHADER_DESC shader_desc;
            pReflection->GetDesc(&shader_desc);

            for (int i = 0; i < shader_desc.BoundResources; i++)
            {
                D3D12_SHADER_INPUT_BIND_DESC  resource_desc;
                pReflection->GetResourceBindingDesc(i, &resource_desc);


                ReadFromReflection(resource_desc, srvCount, uavCount, cbvCount);




            }


            


            return bufferPointer;

        }
        
        void CreatePipelineStateObject()=default;

    };
}
