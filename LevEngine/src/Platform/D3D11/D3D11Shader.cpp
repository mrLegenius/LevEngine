#include "levpch.h"
#include "D3D11Shader.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

#include "Renderer/RenderSettings.h"
#include "Renderer/Shader/ShaderMacros.h"

struct ShaderDefine
{
	String Name;
	String Definition;
};

namespace LevEngine
{
bool CreateShader(ID3DBlob*& shaderBC, const wchar_t* shaderFilepath, Vector<ShaderDefine> defines, ID3DInclude* includes, const char* entrypoint, const char* target);
bool CreatePixelShader(ID3D11PixelShader*& shader, const String& filepath);
bool CreateGeometryShader(ID3D11GeometryShader*& shader, const String& filepath);
bool CreateComputeShader(ID3D11ComputeShader*& shader, const String& filepath);

DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc);
	
D3D11Shader::D3D11Shader(ID3D11Device2* device, const String& filepath, const ShaderType shaderTypes, const ShaderMacros& macros) : Shader(filepath), m_Device(device)
{
	LEV_PROFILE_FUNCTION();

	device->GetImmediateContext2(&m_DeviceContext);

	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == String::npos ? 0 : lastSlash + 1;
	const auto lastDot = filepath.rfind('.');

	const auto count = lastDot == String::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);

	if (shaderTypes & ShaderType::Vertex)
	{
		auto result = CreateVertexShader(m_VertexShader, filepath, macros);
		LEV_CORE_ASSERT(result, "Can't create vertex shader")
	}

	if (shaderTypes & ShaderType::Pixel)
	{
		auto result = CreatePixelShader(m_PixelShader, filepath, macros);
		LEV_CORE_ASSERT(result, "Can't create pixel shader")
	}

	if (shaderTypes & ShaderType::Geometry)
	{
		auto result = CreateGeometryShader(m_GeometryShader, filepath, macros);
		LEV_CORE_ASSERT(result, "Can't create geometry shader")
	}

	if (shaderTypes & ShaderType::Compute)
	{
		auto result = CreateComputeShader(m_ComputeShader, filepath, macros);
		LEV_CORE_ASSERT(result, "Can't create compute shader")
	}

	m_Type = shaderTypes;
}

D3D11Shader::~D3D11Shader()
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

bool CreateShader(ID3DBlob*& shaderBC, const String& shaderFilepath, ShaderMacros defines, ID3DInclude* includes, const char* entrypoint, const char* target)
{
	LEV_PROFILE_FUNCTION();

	Vector<D3D_SHADER_MACRO> shaderDefines;
	shaderDefines.reserve(defines.size() + 1);

	for (auto shaderDefine : defines)
	{
		shaderDefines.push_back(D3D_SHADER_MACRO{
			.Name = shaderDefine.first.c_str(),
			.Definition = shaderDefine.second.c_str() });	
	}
	shaderDefines.push_back({nullptr, nullptr});
	
	const std::wstring widestr = std::wstring(shaderFilepath.begin(), shaderFilepath.end());
	const wchar_t* wide_filepath = widestr.c_str();

	int flags = 0;

	if constexpr (RenderSettings::EnableShaderDebug)
	{
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	}

	ID3DBlob* errorCode = nullptr;
	const auto res = D3DCompileFromFile(wide_filepath,
		shaderDefines.data() /*macros*/,
		includes /*include*/,
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
			const char* compileErrors = static_cast<char*>(errorCode->GetBufferPointer());

			Log::CoreError(compileErrors);

			errorCode->Release();
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			Log::CoreError("{0} is missing shader file", shaderFilepath);
		}

		assert(false);
		return false;
	}

	return true;
}

bool D3D11Shader::CreateVertexShader(ID3D11VertexShader*& shader, const String& filepath, const ShaderMacros& macros)
{
	LEV_PROFILE_FUNCTION();

	ID3DBlob* vertexBC;

	if (!CreateShader(vertexBC, filepath, macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0"))
		return false;

	m_Device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &shader);

	CreateInputLayout(vertexBC);
	CreateShaderParams(ShaderType::Vertex, vertexBC);

	return true;
}

bool D3D11Shader::CreatePixelShader(ID3D11PixelShader*& shader, const String& filepath, const ShaderMacros& macros)
{
	LEV_PROFILE_FUNCTION();

	ID3DBlob* pixelBC;

	if (!CreateShader(pixelBC, filepath, macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0"))
		return false;

	m_Device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &shader);

	CreateShaderParams(ShaderType::Pixel, pixelBC);

	return true;
}

bool D3D11Shader::CreateGeometryShader(ID3D11GeometryShader*& shader, const String& filepath, const ShaderMacros& macros)
{
	LEV_PROFILE_FUNCTION();

	ID3DBlob* geometryBC;

	if (!CreateShader(geometryBC, filepath, macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GSMain", "gs_5_0"))
		return false;

	m_Device->CreateGeometryShader(
		geometryBC->GetBufferPointer(),
		geometryBC->GetBufferSize(),
		nullptr, &shader);

	CreateShaderParams(ShaderType::Geometry, geometryBC);

	return true;
}

bool D3D11Shader::CreateComputeShader(ID3D11ComputeShader*& shader, const String& filepath, const ShaderMacros& macros)
{
	LEV_PROFILE_FUNCTION();

	ID3DBlob* blob;

	if (!CreateShader(blob, filepath, macros, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CSMain", "cs_5_0"))
		return false;

	m_Device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr, &shader);

	CreateShaderParams(ShaderType::Compute, blob);

	return true;
}

void D3D11Shader::CreateInputLayout(ID3DBlob* vertexBlob)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
	auto result = D3DReflect(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
	LEV_ASSERT(SUCCEEDED(result), "Failed to get shader reflection")

	// Query input parameters and build the input layout
	D3D11_SHADER_DESC shaderDescription;
	result = pReflector->GetDesc(&shaderDescription);
	LEV_ASSERT(SUCCEEDED(result), "Failed to get shader description from shader reflector")

	m_InputSemantics.clear();

	const uint32_t numInputParameters = shaderDescription.InputParameters;
	Vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
	for (uint32_t i = 0; i < numInputParameters; ++i)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement;
		D3D11_SIGNATURE_PARAMETER_DESC parameterSignature;

		pReflector->GetInputParameterDesc(i, &parameterSignature);

		inputElement.SemanticName = parameterSignature.SemanticName;
		inputElement.SemanticIndex = parameterSignature.SemanticIndex;
		inputElement.InputSlot = i; // TODO: If using interleaved arrays, then the input slot should be 0.  If using packed arrays, the input slot will vary.
		inputElement.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		inputElement.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // TODO: Figure out how to deal with per-instance data? .. Maybe just use structured buffers to store per-instance data and use the SV_InstanceID as an index in the structured buffer.
		inputElement.InstanceDataStepRate = 0;
		inputElement.Format = GetDXGIFormat(parameterSignature);

		LEV_ASSERT(inputElement.Format != DXGI_FORMAT_UNKNOWN, "Wrong shadet input format");

		inputElements.push_back(inputElement);

		m_InputSemantics.emplace(BufferBinding(inputElement.SemanticName, inputElement.SemanticIndex), i);
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
	auto result = D3DReflect(blob->GetBufferPointer(), blob->GetBufferSize(), IID_ID3D11ShaderReflection, &reflector);
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
		Ref<ShaderParameter> shaderParameter = CreateRef<ShaderParameter>(resourceName, bindDesc.BindPoint, shaderType);
		m_ShaderParameters.emplace(resourceName, shaderParameter);
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
