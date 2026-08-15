#include "levpch.h"
#include "Renderer/Shader/ShaderDiagnostics.h"
#include "D3D11Shader.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

#include "Assets/EngineAssets.h"
#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/ShaderMacros.h"

struct ShaderDefine
{
    String Name;
    String Definition;
};

namespace LevEngine
{
    // D3D_COMPILE_STANDARD_FILE_INCLUDE resolves every include, however deeply nested, against
    // the directory of the top-level shader. That breaks any header that includes a sibling of
    // its own while being pulled in from a subdirectory -- ShaderCommon.hlsl including
    // Registers.hlsli, reached from DebugRender/. This handler resolves relative to the file
    // doing the including, which is what fxc does and what the shaders are written against.
    //
    // Anything it cannot find there is looked up in the engine shader directory, so a shader
    // living in a project can include ShaderCommon.hlsl and friends by their bare name.
    class ShaderIncludeHandler final : public ID3DInclude
    {
    public:
        explicit ShaderIncludeHandler(const String& shaderFilepath)
            : m_ShaderDirectory(Path(shaderFilepath.c_str()).parent_path()) { }

        HRESULT __stdcall Open(D3D_INCLUDE_TYPE, const char* fileName, const void* parentData,
                               const void** outData, UINT* outBytes) override
        {
            const auto parent = m_IncludeDirectories.find(parentData);
            const Path& baseDirectory = parent != m_IncludeDirectories.end() ? parent->second : m_ShaderDirectory;
            Path resolved = baseDirectory / fileName;

            std::ifstream file(resolved, std::ios::binary | std::ios::ate);
            if (!file)
            {
                resolved = EngineResourcesRoot / "Shaders" / fileName;
                file = std::ifstream(resolved, std::ios::binary | std::ios::ate);
            }

            if (!file) return E_FAIL;

            const auto size = static_cast<size_t>(file.tellg());
            file.seekg(0);

            const auto buffer = new char[size];
            file.read(buffer, static_cast<std::streamsize>(size));

            //remember where this header lives so its own includes resolve against it
            m_IncludeDirectories[buffer] = resolved.parent_path();

            *outData = buffer;
            *outBytes = static_cast<UINT>(size);

            return S_OK;
        }

        HRESULT __stdcall Close(const void* data) override
        {
            m_IncludeDirectories.erase(data);
            delete[] static_cast<const char*>(data);

            return S_OK;
        }

    private:
        Path m_ShaderDirectory;
        UnorderedMap<const void*, Path> m_IncludeDirectories;
    };

    bool CreateShader(ID3DBlob*& shaderBC, const wchar_t* shaderFilepath, Vector<ShaderDefine> defines,
                      const char* entrypoint, const char* target);
    bool CreatePixelShader(ID3D11PixelShader*& shader, const String& filepath);
    bool CreateGeometryShader(ID3D11GeometryShader*& shader, const String& filepath);
    bool CreateComputeShader(ID3D11ComputeShader*& shader, const String& filepath);

    DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc);

    D3D11Shader::D3D11Shader(ID3D11Device2* device, const String& filepath,
                             const ShaderMacros& macros) : Shader(filepath, macros), m_Device(device)
    {
        LEV_PROFILE_FUNCTION();

        device->GetImmediateContext2(&m_DeviceContext);

        auto lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == String::npos ? 0 : lastSlash + 1;
        const auto lastDot = filepath.rfind('.');

        const auto count = lastDot == String::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        m_Name = filepath.substr(lastSlash, count);

        CreateShaders(filepath, macros);
    }

    D3D11Shader::~D3D11Shader()
    {
        Clear();
    }

    void D3D11Shader::Bind() const
    {
        for (const auto [_, parameter] : m_ShaderParameters)
            parameter->Bind();

        if (m_VertexShader)
        {
            m_DeviceContext->VSSetShader(m_VertexShader, nullptr, 0);
            m_DeviceContext->IASetInputLayout(m_InputLayout);
        }
        if (m_PixelShader)
            m_DeviceContext->PSSetShader(m_PixelShader, nullptr, 0);
        if (m_GeometryShader)
            m_DeviceContext->GSSetShader(m_GeometryShader, nullptr, 0);
        if (m_ComputeShader)
            m_DeviceContext->CSSetShader(m_ComputeShader, nullptr, 0);
    }

    void D3D11Shader::Unbind() const
    {
        for (const auto [_, parameter] : m_ShaderParameters)
            parameter->Unbind();

        if (m_VertexShader)
        {
            m_DeviceContext->VSSetShader(nullptr, nullptr, 0);
            m_DeviceContext->IASetInputLayout(nullptr);
        }
        if (m_PixelShader)
            m_DeviceContext->PSSetShader(nullptr, nullptr, 0);
        if (m_GeometryShader)
            m_DeviceContext->GSSetShader(nullptr, nullptr, 0);
        if (m_ComputeShader)
            m_DeviceContext->CSSetShader(nullptr, nullptr, 0);
    }

    void D3D11Shader::Reload()
    {
        Clear();
        CreateShaders(m_FilePath, m_Macros);
    }

    void D3D11Shader::Clear()
    {
        if (m_PixelShader)
            m_PixelShader->Release();
        if (m_VertexShader)
            m_VertexShader->Release();
        if (m_GeometryShader)
            m_GeometryShader->Release();
        if (m_ComputeShader)
            m_ComputeShader->Release();
        if (m_InputLayout)
            m_InputLayout->Release();

        //<--- Reset everything, so a failed reload never leaves dangling pointers behind ---<<
        m_PixelShader = nullptr;
        m_VertexShader = nullptr;
        m_GeometryShader = nullptr;
        m_ComputeShader = nullptr;
        m_InputLayout = nullptr;

        m_InputSemantics.clear();
        m_ShaderParameters.clear();
        m_MaterialLayout.Clear();

        m_Type = ShaderType::None;
    }

    void D3D11Shader::CreateShaders(const String& filepath, const ShaderMacros& macros)
    {
        ShaderType existingShaders{};

        if (CreateVertexShader(m_VertexShader, filepath, macros))
            existingShaders = existingShaders | ShaderType::Vertex;

        if (CreatePixelShader(m_PixelShader, filepath, macros))
            existingShaders = existingShaders | ShaderType::Pixel;

        if (CreateGeometryShader(m_GeometryShader, filepath, macros))
            existingShaders = existingShaders | ShaderType::Geometry;

        if (CreateComputeShader(m_ComputeShader, filepath, macros))
            existingShaders = existingShaders | ShaderType::Compute;

        m_Type = existingShaders;

        if (existingShaders == ShaderType::None)
            Log::CoreError("Failed to create any shader stage from {0}", filepath);
    }

    bool CreateShader(ID3DBlob*& shaderBC, const String& shaderFilepath, ShaderMacros defines,
                      const char* entrypoint, const char* target)
    {
        LEV_PROFILE_FUNCTION();

        Vector<D3D_SHADER_MACRO> shaderDefines;
        shaderDefines.reserve(defines.size() + 1);

        for (auto shaderDefine : defines)
        {
            shaderDefines.push_back(D3D_SHADER_MACRO{
                .Name = shaderDefine.first.c_str(),
                .Definition = shaderDefine.second.c_str()
            });
        }
        shaderDefines.push_back({nullptr, nullptr});

        const std::wstring widestr = std::wstring(shaderFilepath.begin(), shaderFilepath.end());
        const wchar_t* wide_filepath = widestr.c_str();

        int flags = 0;

        if constexpr (RenderSettings::EnableShaderDebug)
        {
            flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
        }

        ShaderIncludeHandler includeHandler{shaderFilepath};

        ID3DBlob* errorCode = nullptr;
        const auto res = D3DCompileFromFile(wide_filepath,
                                            shaderDefines.data() /*macros*/,
                                            &includeHandler /*include*/,
                                            entrypoint,
                                            target,
                                            flags,
                                            0,
                                            &shaderBC,
                                            &errorCode);

        if (FAILED(res))
        {
            // If the shader failed to compile it should have written something to the error message.
            if (errorCode)
            {
                String errorMessage(static_cast<char*>(errorCode->GetBufferPointer()), errorCode->GetBufferSize());

                //<--- X3501 is 'entrypoint not found'. A file is not required to have every stage ---<<
                if (errorMessage.find("X3501") == String::npos)
                {
                    const char* compileErrors = static_cast<char*>(errorCode->GetBufferPointer());
                    Log::CoreError(compileErrors);
                    ShaderDiagnostics::Record(shaderFilepath, target, errorMessage);
                }

                errorCode->Release();
            }
            // If there was  nothing in the error message then it simply could not find the shader file itself.
            else
            {
                Log::CoreError("{0} is missing shader file", shaderFilepath);
                ShaderDiagnostics::Record(shaderFilepath, target, "Shader file not found");
            }

            return false;
        }

        return true;
    }

    bool D3D11Shader::CreateVertexShader(ID3D11VertexShader*& shader, const String& filepath,
                                         const ShaderMacros& macros)
    {
        LEV_PROFILE_FUNCTION();

        ID3DBlob* vertexBC = nullptr;

        if (!CreateShader(vertexBC, filepath, macros, "VSMain", "vs_5_0"))
            return false;

        m_Device->CreateVertexShader(
            vertexBC->GetBufferPointer(),
            vertexBC->GetBufferSize(),
            nullptr, &shader);

        CreateInputLayout(vertexBC);
        CreateShaderParams(ShaderType::Vertex, vertexBC);

        vertexBC->Release();

        return true;
    }

    bool D3D11Shader::CreatePixelShader(ID3D11PixelShader*& shader, const String& filepath, const ShaderMacros& macros)
    {
        LEV_PROFILE_FUNCTION();

        ID3DBlob* pixelBC = nullptr;

        if (!CreateShader(pixelBC, filepath, macros, "PSMain", "ps_5_0"))
            return false;

        m_Device->CreatePixelShader(
            pixelBC->GetBufferPointer(),
            pixelBC->GetBufferSize(),
            nullptr, &shader);

        CreateShaderParams(ShaderType::Pixel, pixelBC);

        pixelBC->Release();

        return true;
    }

    bool D3D11Shader::CreateGeometryShader(ID3D11GeometryShader*& shader, const String& filepath,
                                           const ShaderMacros& macros)
    {
        LEV_PROFILE_FUNCTION();

        ID3DBlob* geometryBC = nullptr;

        if (!CreateShader(geometryBC, filepath, macros, "GSMain", "gs_5_0"))
            return false;

        m_Device->CreateGeometryShader(
            geometryBC->GetBufferPointer(),
            geometryBC->GetBufferSize(),
            nullptr, &shader);

        CreateShaderParams(ShaderType::Geometry, geometryBC);

        geometryBC->Release();

        return true;
    }

    bool D3D11Shader::CreateComputeShader(ID3D11ComputeShader*& shader, const String& filepath,
                                          const ShaderMacros& macros)
    {
        LEV_PROFILE_FUNCTION();

        ID3DBlob* blob = nullptr;

        if (!CreateShader(blob, filepath, macros, "CSMain", "cs_5_0"))
            return false;

        m_Device->CreateComputeShader(
            blob->GetBufferPointer(),
            blob->GetBufferSize(),
            nullptr, &shader);

        CreateShaderParams(ShaderType::Compute, blob);

        blob->Release();

        return true;
    }

    void D3D11Shader::CreateInputLayout(ID3DBlob* vertexBlob)
    {
        Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
        auto result = D3DReflect(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(),
                                 IID_ID3D11ShaderReflection, &pReflector);
        LEV_ASSERT(SUCCEEDED(result), "Failed to get shader reflection")

        // Query input parameters and build the input layout
        D3D11_SHADER_DESC shaderDescription;
        result = pReflector->GetDesc(&shaderDescription);
        LEV_ASSERT(SUCCEEDED(result), "Failed to get shader description from shader reflector")

        m_InputSemantics.clear();

        const uint32_t numInputParameters = shaderDescription.InputParameters;
        Vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
        uint32_t slot = 0;
        for (uint32_t i = 0; i < numInputParameters; ++i)
        {
            D3D11_INPUT_ELEMENT_DESC inputElement;
            D3D11_SIGNATURE_PARAMETER_DESC parameterSignature;

            pReflector->GetInputParameterDesc(i, &parameterSignature);

            //<--- System values (SV_InstanceID, SV_VertexID) are generated by the input assembler,
            //     not fed from a vertex buffer. Describing one as an input element fails layout
            //     creation, so they are skipped and never take up a slot. ---<<
            if (parameterSignature.SystemValueType != D3D_NAME_UNDEFINED) continue;

            inputElement.SemanticName = parameterSignature.SemanticName;
            inputElement.SemanticIndex = parameterSignature.SemanticIndex;
            inputElement.InputSlot = slot;
            // TODO: If using interleaved arrays, then the input slot should be 0.  If using packed arrays, the input slot will vary.
            inputElement.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            inputElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            // Per-instance data does not go through the input assembler at all: it lives in a
            // structured buffer indexed by SV_InstanceID. See Renderer3D::DrawMeshInstanced.
            inputElement.InstanceDataStepRate = 0;
            inputElement.Format = GetDXGIFormat(parameterSignature);

            LEV_ASSERT(inputElement.Format != DXGI_FORMAT_UNKNOWN, "Wrong shadet input format");

            inputElements.push_back(inputElement);

            m_InputSemantics.emplace(BufferBinding(inputElement.SemanticName, inputElement.SemanticIndex), slot);

            ++slot;
        }

        if (inputElements.size() > 0)
        {
            result = m_Device->CreateInputLayout(inputElements.data(),
                                                 static_cast<UINT>(inputElements.size()),
                                                 vertexBlob->GetBufferPointer(),
                                                 vertexBlob->GetBufferSize(),
                                                 &m_InputLayout);

            LEV_ASSERT(SUCCEEDED(result), "Failed to create input layout")
        }
    }

    void D3D11Shader::CreateShaderParams(ShaderType shaderType, ID3DBlob* blob)
    {
        Microsoft::WRL::ComPtr<ID3D11ShaderReflection> reflector;
        auto result = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection,
                                 &reflector);
        LEV_ASSERT(SUCCEEDED(result), "Failed to get shader reflection")

        // Query input parameters and build the input layout
        D3D11_SHADER_DESC shaderDescription;
        result = reflector->GetDesc(&shaderDescription);
        LEV_ASSERT(SUCCEEDED(result), "Failed to get shader description from shader reflector")

        // Query Resources that are bound to the shader.
        for (UINT i = 0; i < shaderDescription.BoundResources; ++i)
        {
            D3D11_SHADER_INPUT_BIND_DESC bindDesc;
            reflector->GetResourceBindingDesc(i, &bindDesc);
            String resourceName = bindDesc.Name;

            /*ShaderParameter::Type parameterType = ShaderParameter::Type::Invalid;
    
            switch (bindDesc.Type)
            {
            case D3D_SIT_TEXTURE:
                parameterType = ShaderParameter::Type::Texture;
                break;
            case D3D_SIT_SAMPLER:
                parameterType = ShaderParameter::Type::Sampler;
                break;
            case D3D_SIT_CBUFFER:
            case D3D_SIT_STRUCTURED:
                parameterType = ShaderParameter::Type::Buffer;
                break;
            case D3D_SIT_UAV_RWSTRUCTURED:
                parameterType = ShaderParameter::Type::RWBuffer;
                break;
            case D3D_SIT_UAV_RWTYPED:
                parameterType = ShaderParameter::Type::RWTexture;
                break;
            }*/

            // Create an empty shader parameter that should be filled-in by the application.
            Ref<ShaderParameter> shaderParameter = CreateRef<ShaderParameter>(
                resourceName, bindDesc.BindPoint, shaderType);
            m_ShaderParameters.emplace(resourceName, shaderParameter);

            //<--- Textures in the material range are the ones a material is expected to fill ---<<
            if (bindDesc.Type == D3D_SIT_TEXTURE && bindDesc.BindPoint < k_MaterialTextureSlotCount)
            {
                const auto isKnown = eastl::any_of(m_MaterialLayout.Textures.begin(),
                                                   m_MaterialLayout.Textures.end(),
                                                   [&resourceName](const ShaderTextureProperty& texture)
                                                   {
                                                       return texture.Name == resourceName;
                                                   });

                if (!isKnown)
                    m_MaterialLayout.Textures.push_back({resourceName, bindDesc.BindPoint});
            }
        }

        ReflectMaterialLayout(reflector.Get());
    }

    static ShaderPropertyType GetPropertyType(const D3D11_SHADER_TYPE_DESC& desc)
    {
        //<--- Matrices, arrays and anything else a material cannot edit are left out ---<<
        if (desc.Class != D3D_SVC_SCALAR && desc.Class != D3D_SVC_VECTOR) return ShaderPropertyType::Unknown;
        if (desc.Elements > 0) return ShaderPropertyType::Unknown;

        switch (desc.Type)
        {
        case D3D_SVT_FLOAT:
            switch (desc.Columns)
            {
            case 1: return ShaderPropertyType::Float;
            case 2: return ShaderPropertyType::Float2;
            case 3: return ShaderPropertyType::Float3;
            case 4: return ShaderPropertyType::Float4;
            default: return ShaderPropertyType::Unknown;
            }
        case D3D_SVT_INT:
        case D3D_SVT_UINT:
            return desc.Columns == 1 ? ShaderPropertyType::Int : ShaderPropertyType::Unknown;
        case D3D_SVT_BOOL:
            return desc.Columns == 1 ? ShaderPropertyType::Bool : ShaderPropertyType::Unknown;
        default:
            return ShaderPropertyType::Unknown;
        }
    }

    // Flattens one constant buffer field into the layout. A struct contributes its members under
    // their own names -- shaders wrap their properties in one, and 'tint' reads better in the
    // inspector and in the material file than 'material.tint'.
    static void ReflectProperty(ID3D11ShaderReflectionType* type, const String& name, const uint32_t offset,
                                ShaderMaterialLayout& layout)
    {
        D3D11_SHADER_TYPE_DESC typeDescription;
        if (FAILED(type->GetDesc(&typeDescription))) return;

        if (typeDescription.Class == D3D_SVC_STRUCT)
        {
            for (UINT i = 0; i < typeDescription.Members; ++i)
            {
                auto* member = type->GetMemberTypeByIndex(i);
                const char* memberName = type->GetMemberTypeName(i);
                if (!member || !memberName) continue;

                D3D11_SHADER_TYPE_DESC memberDescription;
                if (FAILED(member->GetDesc(&memberDescription))) continue;

                ReflectProperty(member, memberName, offset + memberDescription.Offset, layout);
            }

            return;
        }

        const auto propertyType = GetPropertyType(typeDescription);
        if (propertyType == ShaderPropertyType::Unknown) return;

        layout.Properties.push_back({name, propertyType, offset});
    }

    void D3D11Shader::ReflectMaterialLayout(ID3D11ShaderReflection* reflector)
    {
        //<--- Both stages declare the same buffer, so the first one that has it wins ---<<
        if (m_MaterialLayout.BufferSize != 0) return;

        auto* buffer = reflector->GetConstantBufferByName(k_MaterialConstantBufferName);
        if (!buffer) return;

        //<--- GetConstantBufferByName never returns null, GetDesc is what fails for a missing buffer ---<<
        D3D11_SHADER_BUFFER_DESC bufferDescription;
        if (FAILED(buffer->GetDesc(&bufferDescription))) return;

        m_MaterialLayout.BufferSize = bufferDescription.Size;

        for (UINT i = 0; i < bufferDescription.Variables; ++i)
        {
            auto* variable = buffer->GetVariableByIndex(i);
            if (!variable) continue;

            D3D11_SHADER_VARIABLE_DESC variableDescription;
            if (FAILED(variable->GetDesc(&variableDescription))) continue;

            auto* type = variable->GetType();
            if (!type) continue;

            ReflectProperty(type, variableDescription.Name, variableDescription.StartOffset, m_MaterialLayout);
        }
    }

    ShaderParameter& D3D11Shader::GetShaderParameterByName(const String& name) const
    {
        const auto it = m_ShaderParameters.find(name);
        if (it != m_ShaderParameters.end())
            return *it->second;

        static ShaderParameter invalid;

        return invalid;
    }

    bool D3D11Shader::HasSemantic(const BufferBinding& binding)
    {
        const auto it = m_InputSemantics.find(binding);
        return it != m_InputSemantics.end();
    }

    uint32_t D3D11Shader::GetSlotIdBySemantic(const BufferBinding& binding)
    {
        const auto it = m_InputSemantics.find(binding);
        if (it != m_InputSemantics.end())
            return it->second;

        LEV_THROW("Failed to get binding in shader")
    }

    DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc)
    {
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        if (paramDesc.Mask == 1) // 1 component
        {
            switch (paramDesc.ComponentType)
            {
            case D3D_REGISTER_COMPONENT_UINT32:
                {
                    format = DXGI_FORMAT_R32_UINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_SINT32:
                {
                    format = DXGI_FORMAT_R32_SINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_FLOAT32:
                {
                    format = DXGI_FORMAT_R32_FLOAT;
                }
                break;
            }
        }
        else if (paramDesc.Mask <= 3) // 2 components
        {
            switch (paramDesc.ComponentType)
            {
            case D3D_REGISTER_COMPONENT_UINT32:
                {
                    format = DXGI_FORMAT_R32G32_UINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_SINT32:
                {
                    format = DXGI_FORMAT_R32G32_SINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_FLOAT32:
                {
                    format = DXGI_FORMAT_R32G32_FLOAT;
                }
                break;
            }
        }
        else if (paramDesc.Mask <= 7) // 3 components
        {
            switch (paramDesc.ComponentType)
            {
            case D3D_REGISTER_COMPONENT_UINT32:
                {
                    format = DXGI_FORMAT_R32G32B32_UINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_SINT32:
                {
                    format = DXGI_FORMAT_R32G32B32_SINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_FLOAT32:
                {
                    format = DXGI_FORMAT_R32G32B32_FLOAT;
                }
                break;
            }
        }
        else if (paramDesc.Mask <= 15) // 4 components
        {
            switch (paramDesc.ComponentType)
            {
            case D3D_REGISTER_COMPONENT_UINT32:
                {
                    format = DXGI_FORMAT_R32G32B32A32_UINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_SINT32:
                {
                    format = DXGI_FORMAT_R32G32B32A32_SINT;
                }
                break;
            case D3D_REGISTER_COMPONENT_FLOAT32:
                {
                    format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                }
                break;
            }
        }

        return format;
    }
}
