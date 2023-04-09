#pragma once
#include <d3d11.h>
#include <string>

#include "Buffer.h"

class D3D11Shader
{
public:
	enum class ShaderType
	{
		None = 0,
		Vertex = 1,
		Geometry = 2,
		Pixel = 4,
	};

	explicit D3D11Shader(const std::string& filepath);
	D3D11Shader(const std::string& filepath, ShaderType shaderTypes);
	~D3D11Shader();
	void Bind() const;
	void Unbind() const;

	void SetLayout(const BufferLayout& layout);
	[[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }

private:
	std::string m_FilePath;
	std::string m_Name;

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3DBlob* m_VertexBC = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11GeometryShader* m_GeometryShader = nullptr;

	ID3D11InputLayout* m_InputLayout = nullptr;
	BufferLayout m_Layout;
};

inline D3D11Shader::ShaderType operator|(const D3D11Shader::ShaderType a, const D3D11Shader::ShaderType b)
{
	return static_cast<D3D11Shader::ShaderType>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(const D3D11Shader::ShaderType a, const D3D11Shader::ShaderType b)
{
	return static_cast<int>(a) & static_cast<int>(b);
}