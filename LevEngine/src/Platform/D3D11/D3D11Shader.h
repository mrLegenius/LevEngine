#pragma once
#include <d3d11.h>

#include "Renderer/Shader/Shader.h"

namespace LevEngine
{
class D3D11Shader : public Shader
{
public:
	
	D3D11Shader(const String& filepath, ShaderType shaderTypes);
	~D3D11Shader() override;

	void Bind() const override;
	void Unbind() const override;

	[[nodiscard]] bool HasSemantic(const BufferBinding& binding) override;
	[[nodiscard]] uint32_t GetSlotIdBySemantic(const BufferBinding& binding) override;
	[[nodiscard]] ShaderParameter& GetShaderParameterByName(const String& name) const override;
private:
	void CreateInputLayout(ID3DBlob* vertexBlob);
	void CreateShaderParams(ShaderType shaderType, ID3DBlob* blob);

	bool CreateVertexShader(ID3D11VertexShader*& shader, const String& filepath);
	bool CreatePixelShader(ID3D11PixelShader*& shader, const String& filepath);
	bool CreateGeometryShader(ID3D11GeometryShader*& shader, const String& filepath);
	bool CreateComputeShader(ID3D11ComputeShader*& shader, const String& filepath);

	ID3D11VertexShader* m_VertexShader = nullptr;
	ID3D11PixelShader* m_PixelShader = nullptr;
	ID3D11GeometryShader* m_GeometryShader = nullptr;
	ID3D11ComputeShader* m_ComputeShader = nullptr;

	ID3D11InputLayout* m_InputLayout = nullptr;

	Map<BufferBinding, uint32_t> m_InputSemantics;
	Map<String, Ref<ShaderParameter>> m_ShaderParameters;
};
}
