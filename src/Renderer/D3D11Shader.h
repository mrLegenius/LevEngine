#pragma once
#include <d3d11.h>
#include <string>

#include "Buffer.h"

enum class ShaderType
{
	None = 0,
	Vertex = 1,
	Geometry = 2,
	Pixel = 4,
	Compute = 8,
};

class D3D11Shader
{
public:

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
	ID3D11ComputeShader* m_ComputeShader = nullptr;

	ID3D11InputLayout* m_InputLayout = nullptr;
	BufferLayout m_Layout;
};

inline ShaderType operator|(const ShaderType a, const ShaderType b)
{
	return static_cast<ShaderType>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(const ShaderType a, const ShaderType b)
{
	return static_cast<int>(a) & static_cast<int>(b);
}