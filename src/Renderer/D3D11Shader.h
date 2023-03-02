#pragma once
#include <d3d11.h>
#include <string>

#include "Buffer.h"

class D3D11Shader
{
public:
	explicit D3D11Shader(const std::string& filepath);
	~D3D11Shader();
	void Bind() const;

	void SetLayout(const BufferLayout& layout);
	[[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }

private:
	std::string m_FilePath;
	std::string m_Name;

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3DBlob* m_VertexBC = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;

	ID3D11InputLayout* m_InputLayout = nullptr;
	BufferLayout m_Layout;
};

