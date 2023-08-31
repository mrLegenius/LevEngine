#pragma once
#include <d3d11.h>

#include "Renderer/Shader.h"

namespace LevEngine
{
class D3D11Shader : public Shader
{
public:

	explicit D3D11Shader(const std::string& filepath);
	D3D11Shader(const std::string& filepath, Type shaderTypes);
	~D3D11Shader() override;

	void Bind() const override;
	void Unbind() const override;

	[[nodiscard]] bool HasSemantic(const BufferBinding& binding) override;
	[[nodiscard]] uint32_t GetSlotIdBySemantic(const BufferBinding& binding) override;
private:

	void CreateInputLayout();

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3DBlob* m_VertexBC = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11GeometryShader* m_GeometryShader = nullptr;
	ID3D11ComputeShader* m_ComputeShader = nullptr;

	ID3D11InputLayout* m_InputLayout = nullptr;

	std::map<BufferBinding, uint32_t> m_InputSemantics;
};
}
