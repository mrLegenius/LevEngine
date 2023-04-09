#include "D3D11Shader.h"

#include <cassert>
#include <d3dcompiler.h>
#include <iostream>
#include <wrl/client.h>

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

bool CreateShader(ID3DBlob*& shaderBC, const wchar_t* shaderFilepath, D3D_SHADER_MACRO defines[], ID3DInclude* includes, const char* entrypoint, const char* target);
bool CreateVertexShader(ID3D11VertexShader*& shader, ID3DBlob*& vertexBC, const std::string& filepath);
bool CreatePixelShader(ID3D11PixelShader*& shader, const std::string& filepath);
bool CreateGeometryShader(ID3D11GeometryShader*& shader, const std::string& filepath);

static DXGI_FORMAT ParseShaderDataTypeToD3D11DataType(const ShaderDataType type)
{
	switch (type) {
	case ShaderDataType::None: return DXGI_FORMAT_UNKNOWN;
	case ShaderDataType::Float: return DXGI_FORMAT_R32_FLOAT;
	case ShaderDataType::Float2: return DXGI_FORMAT_R32G32_FLOAT;
	case ShaderDataType::Float3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case ShaderDataType::Float4:
	case ShaderDataType::Mat3:
	case ShaderDataType::Mat4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ShaderDataType::Int: return DXGI_FORMAT_R32_SINT;
	case ShaderDataType::Int2: return DXGI_FORMAT_R32G32_SINT;
	case ShaderDataType::Int3: return DXGI_FORMAT_R32G32B32_SINT;
	case ShaderDataType::Int4: return DXGI_FORMAT_R32G32B32A32_SINT;
	case ShaderDataType::Bool: return DXGI_FORMAT_R8_UINT;
	default:
		throw std::exception("Unknown ShaderDataType!");
	}
}

D3D11Shader::D3D11Shader(const std::string& filepath) : D3D11Shader(filepath, ShaderType::Vertex | ShaderType::Pixel) { }
D3D11Shader::D3D11Shader(const std::string& filepath, ShaderType shaderTypes)
{
	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	const auto lastDot = filepath.rfind('.');

	const auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);

	if (shaderTypes & ShaderType::Vertex)
	{
		auto result = CreateVertexShader(m_VertexShader, m_VertexBC, filepath);
		assert(result);
	}

	if (shaderTypes & ShaderType::Pixel)
	{
		auto result = CreatePixelShader(m_PixelShader, filepath);
		assert(result);
	}

	if (shaderTypes & ShaderType::Geometry)
	{
		auto result = CreateGeometryShader(m_GeometryShader, filepath);
		assert(result);
	}
}

D3D11Shader::~D3D11Shader()
{
	if (m_PixelShader)
		m_PixelShader->Release();
	if (m_VertexShader)
		m_VertexShader->Release();
	if (m_GeometryShader)
		m_GeometryShader->Release();

	m_VertexBC->Release();
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
	context->IASetInputLayout(m_InputLayout);
}

void D3D11Shader::Unbind() const
{
	context->VSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->IASetInputLayout(nullptr);
}

bool CreateShader(ID3DBlob*& shaderBC, const std::string& shaderFilepath, D3D_SHADER_MACRO defines[], ID3DInclude* includes, const char* entrypoint, const char* target)
{
	const std::wstring widestr = std::wstring(shaderFilepath.begin(), shaderFilepath.end());
	const wchar_t* wide_filepath = widestr.c_str();

	ID3DBlob* errorCode = nullptr;
	const auto res = D3DCompileFromFile(wide_filepath,
		defines /*macros*/,
		includes /*include*/,
		entrypoint,
		target,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shaderBC,
		&errorCode);

	if (FAILED(res))
	{
		// If the shader failed to compile it should have written something to the error message.
		if (errorCode)
		{
			const char* compileErrors = static_cast<char*>(errorCode->GetBufferPointer());

			std::cout << compileErrors << std::endl;
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			std::cout << shaderFilepath << " Missing shader file" << std::endl;
		}

		return false;
	}

	return true;
}

bool CreateVertexShader(ID3D11VertexShader*& shader, ID3DBlob*& vertexBC, const std::string& filepath)
{
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
	ID3DBlob* pixelBC;
	D3D_SHADER_MACRO defines[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr};

	if (!CreateShader(pixelBC, filepath, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0"))
		return false;

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &shader);

	return true;
}

bool CreateGeometryShader(ID3D11GeometryShader*& shader, const std::string& filepath)
{
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

void D3D11Shader::SetLayout(const BufferLayout& layout)
{
	m_Layout = layout;
	const auto& elements = layout.GetElements();
	auto* input = new D3D11_INPUT_ELEMENT_DESC[elements.size()];

	for (uint32_t i = 0; i < elements.size(); i++)
	{
		input[i] = D3D11_INPUT_ELEMENT_DESC{
			elements[i].name.c_str(),
			0,
			ParseShaderDataTypeToD3D11DataType(elements[i].type),
			0,
			i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };
	}

	device->CreateInputLayout(
		input,
		elements.size(),
		m_VertexBC->GetBufferPointer(),
		m_VertexBC->GetBufferSize(),
		&m_InputLayout);

	delete[] input;
}
