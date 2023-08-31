#pragma once
#include <d3d11.h>
#include "Renderer/Shader.h"

namespace LevEngine
{
class D3D11Shader : public Shader
{
public:

	explicit D3D11Shader(const std::string& filepath);
	D3D11Shader(const std::string& filepath, ShaderType shaderTypes);
	~D3D11Shader() override;

	void Bind() const override;
	void Unbind() const override;

	[[nodiscard]] bool HasSemantic(const BufferBinding& binding) override;
	[[nodiscard]] uint32_t GetSlotIdBySemantic(const BufferBinding& binding) override;
	[[nodiscard]] ShaderParameter& GetShaderParameterByName(const std::string& name) const override;
private:
	void CreateInputLayout(ID3DBlob* vertexBlob);
	void CreateShaderParams(ShaderType shaderType, ID3DBlob* blob);

	bool CreateVertexShader(ID3D11VertexShader*& shader, const std::string& filepath);
	bool CreatePixelShader(ID3D11PixelShader*& shader, const std::string& filepath);
	bool CreateGeometryShader(ID3D11GeometryShader*& shader, const std::string& filepath);
	bool CreateComputeShader(ID3D11ComputeShader*& shader, const std::string& filepath);

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11GeometryShader* m_GeometryShader = nullptr;
	ID3D11ComputeShader* m_ComputeShader = nullptr;

	ID3D11InputLayout* m_InputLayout = nullptr;

	std::map<BufferBinding, uint32_t> m_InputSemantics;
	std::map<std::string, Ref<ShaderParameter>> m_ShaderParameters;
};
}
