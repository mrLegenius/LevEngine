#pragma once
#include <d3d11_2.h>

#include "Renderer/Pipeline/IndexBuffer.h"

namespace LevEngine
{
class D3D11IndexBuffer final : public IndexBuffer
{
public:
	D3D11IndexBuffer(ID3D11Device2* device, const uint32_t* indices, uint32_t count);
	~D3D11IndexBuffer() override;

	void Bind() const override;
	void Unbind() const override;

private:
	ID3D11Buffer* m_Buffer = nullptr;

	ID3D11DeviceContext2* m_DeviceContext;
};
}
