#pragma once
#include <d3d11.h>

#include "Renderer/IndexBuffer.h"

namespace LevEngine
{
class D3D11IndexBuffer final : public IndexBuffer
{
public:
	D3D11IndexBuffer(const uint32_t* indices, uint32_t count);
	~D3D11IndexBuffer() override;

	void Bind() const override;
	void Unbind() const override;

private:
	ID3D11Buffer* m_Buffer = nullptr;
};
}
