#include "D3D11Shader.h"

#include <d3dcompiler.h>
#include <iostream>
#include <wrl/client.h>

extern ID3D11DeviceContext* context;
extern HWND hWnd;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

bool CreateShader(ID3DBlob*& shaderBC, const wchar_t* shaderFilepath, D3D_SHADER_MACRO defines[], ID3DInclude* includes, const char* entrypoint, const char* target);
bool CreateVertexShader(ID3D11VertexShader*& shader, ID3DBlob*& vertexBC, const wchar_t* shaderFilepath);
bool CreatePixelShader(ID3D11PixelShader*& shader, const wchar_t* shaderFilepath);

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

D3D11Shader::D3D11Shader(const std::string& filepath)
{
	auto lastSlash = filepath.find_last_of("/\\");
	lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
	const auto lastDot = filepath.rfind('.');

	const auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
	m_Name = filepath.substr(lastSlash, count);

	const std::wstring widestr = std::wstring(filepath.begin(), filepath.end());
	const wchar_t* wide_filepath = widestr.c_str();

	CreateVertexShader(m_VertexShader, m_VertexBC, wide_filepath);
	CreatePixelShader(m_PixelShader, wide_filepath);
}

void D3D11Shader::Bind() const
{
	context->VSSetShader(m_VertexShader, nullptr, 0);
	context->PSSetShader(m_PixelShader, nullptr, 0);
	context->IASetInputLayout(m_InputLayout);
}

bool CreateShader(ID3DBlob*& shaderBC, const wchar_t* shaderFilepath, D3D_SHADER_MACRO defines[], ID3DInclude* includes, const char* entrypoint, const char* target)
{
	ID3DBlob* errorCode = nullptr;
	const auto res = D3DCompileFromFile(shaderFilepath,
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
			MessageBox(hWnd, shaderFilepath, L"Missing Shader File", MB_OK);
		}

		return false;
	}

	return true;
}

bool CreateVertexShader(ID3D11VertexShader*& shader, ID3DBlob*& vertexBC, const wchar_t* shaderFilepath)
{
	if (!CreateShader(vertexBC, shaderFilepath, nullptr, nullptr, "VSMain", "vs_5_0"))
		return false;

	device->CreateVertexShader(
		vertexBC->GetBufferPointer(),
		vertexBC->GetBufferSize(),
		nullptr, &shader);

	return true;
}

bool CreatePixelShader(ID3D11PixelShader*& shader, const wchar_t* shaderFilepath)
{
	ID3DBlob* pixelBC;
	D3D_SHADER_MACRO defines[] = { "TEST", "1", "TCOLOR", "float4(0.0f, 1.0f, 0.0f, 1.0f)", nullptr, nullptr };

	if (!CreateShader(pixelBC, shaderFilepath, defines, nullptr, "PSMain", "ps_5_0"))
		return false;

	device->CreatePixelShader(
		pixelBC->GetBufferPointer(),
		pixelBC->GetBufferSize(),
		nullptr, &shader);

	return true;
}


void D3D11Shader::SetLayout(const BufferLayout& layout)
{
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
}