#include "levpch.h"
#include "D3D11Shader.h"

#include <d3dcompiler.h>
#include <wrl/client.h>

#include "Renderer/RenderSettings.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

bool CreateShader(ID3DBlob*& shaderBC, const wchar_t* shaderFilepath, D3D_SHADER_MACRO defines[], ID3DInclude* includes, const char* entrypoint, const char* target);
bool CreateVertexShader(ID3D11VertexShader*& shader, ID3DBlob*& vertexBC, const std::string& filepath);
bool CreatePixelShader(ID3D11PixelShader*& shader, const std::string& filepath);
bool CreateGeometryShader(ID3D11GeometryShader*& shader, const std::string& filepath);
bool CreateComputeShader(ID3D11ComputeShader*& shader, const std::string& filepath);

DXGI_FORMAT GetDXGIFormat(const D3D11_SIGNATURE_PARAMETER_DESC& paramDesc);

D3D11Shader::D3D11Shader(const std::string& filepath) : D3D11Shader(filepath, Type::Vertex | Type::Pixel) { }
D3D11Shader::D3D11Shader(const std::string& filepath, const Type shaderTypes) : Shader(filepath)
{
	LEV_PROFILE_FUNCTION();

	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	const auto lastDot = filepath.rfind('.');

	const auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);

	if (shaderTypes & Type::Vertex)
	{
		auto result = CreateVertexShader(m_VertexShader, m_VertexBC, filepath);
		LEV_CORE_ASSERT(result, "Can't create vertex shader")
		CreateInputLayout();
	}

	if (shaderTypes & Type::Pixel)
	{
		auto result = CreatePixelShader(m_PixelShader, filepath);
		LEV_CORE_ASSERT(result, "Can't create pixel shader")
	}

	if (shaderTypes & Type::Geometry)
	{
		auto result = CreateGeometryShader(m_GeometryShader, filepath);
		LEV_CORE_ASSERT(result, "Can't create geometry shader")
	}

	if (shaderTypes & Type::Compute)
	{
		auto result = CreateComputeShader(m_ComputeShader, filepath);
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
		m_VertexBC->Release();
	if (m_InputLayout)
		m_InputLayout->Release();
}

void D3D11Shader::Bind() const
{
	if (m_VertexShader)
		context->VSSetShader(m_VertexShader, nullptr, 0);
	if (m_PixelShader)
		context->PSSetShader(m_PixelShader, nullptr, 0);
	if (m_GeometryShader)
		context->GSSetShader(m_GeometryShader, nullptr, 0);
	if (m_ComputeShader)
		context->CSSetShader(m_ComputeShader, nullptr, 0);
	if (m_InputLayout)
		context->IASetInputLayout(m_InputLayout);
}

void D3D11Shader::Unbind() const
{
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->CSSetShader(nullptr, nullptr, 0);
	context->IASetInputLayout(nullptr);
}

bool CreateShader(ID3DBlob*& shaderBC, const std::string& shaderFilepath, D3D_SHADER_MACRO defines[], ID3DInclude* includes, const char* entrypoint, const char* target)
{
	LEV_PROFILE_FUNCTION();

	const std::wstring widestr = std::wstring(shaderFilepath.begin(), shaderFilepath.end());
	const wchar_t* wide_filepath = widestr.c_str();

	int flags = 0;

	if constexpr (RenderSettings::EnableShaderDebug)
	{
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	}

	ID3DBlob* errorCode = nullptr;
	const auto res = D3DCompileFromFile(wide_filepath,
		defines /*macros*/,
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

bool CreateVertexShader(ID3D11VertexShader*& shader, ID3DBlob*& vertexBC, const std::string& filepath)
{
	LEV_PROFILE_FUNCTION();

	if (!CreateShader(vertexBC, filepath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0"))
		return false;

	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &shader);

	return true;
}

bool CreatePixelShader(ID3D11PixelShader*& shader, const std::string& filepath)
{
	LEV_PROFILE_FUNCTION();

	ID3DBlob* pixelBC;

	if (!CreateShader(pixelBC, filepath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0"))
		return false;

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &shader);

	return true;
}

bool CreateGeometryShader(ID3D11GeometryShader*& shader, const std::string& filepath)
{
	LEV_PROFILE_FUNCTION();

	ID3DBlob* geometryBC;
	D3D_SHADER_MACRO defines[] = { nullptr, nullptr };

	if (!CreateShader(geometryBC, filepath, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GSMain", "gs_5_0"))
		return false;

	device->CreateGeometryShader(
		geometryBC->GetBufferPointer(),
		geometryBC->GetBufferSize(),
		nullptr, &shader);

	return true;
}

bool CreateComputeShader(ID3D11ComputeShader*& shader, const std::string& filepath)
{
	LEV_PROFILE_FUNCTION();

	ID3DBlob* blob;
	D3D_SHADER_MACRO defines[] = { nullptr, nullptr };

	if (!CreateShader(blob, filepath, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "CSMain", "cs_5_0"))
		return false;

	device->CreateComputeShader(
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		nullptr, &shader);

	return true;
}

void D3D11Shader::CreateInputLayout()
{
	Microsoft::WRL::ComPtr<ID3D11ShaderReflection> pReflector;
	auto result = D3DReflect(m_VertexBC->GetBufferPointer(), m_VertexBC->GetBufferSize(), IID_ID3D11ShaderReflection, &pReflector);
	LEV_ASSERT(SUCCEEDED(result), "Failed to get shader reflection")

	// Query input parameters and build the input layout
	D3D11_SHADER_DESC shaderDescription;
	result = pReflector->GetDesc(&shaderDescription);
	LEV_ASSERT(SUCCEEDED(result), "Failed to get shader description from shader reflector")

	m_InputSemantics.clear();

	const uint32_t numInputParameters = shaderDescription.InputParameters;
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
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
		result = device->CreateInputLayout(inputElements.data(), 
			static_cast<UINT>(inputElements.size()), 
			m_VertexBC->GetBufferPointer(), 
			m_VertexBC->GetBufferSize(),
			&m_InputLayout);

		LEV_ASSERT(SUCCEEDED(result), "Failed to create input layout")
	}
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