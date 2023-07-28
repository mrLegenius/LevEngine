#pragma once
#include <d3d11.h>

#include "Renderer/VertexBuffer.h"

namespace LevEngine
{
class D3D11VertexBuffer final : public VertexBuffer
{
public:
	explicit D3D11VertexBuffer(uint32_t size);
	D3D11VertexBuffer(const float* vertices, uint32_t size);
	~D3D11VertexBuffer() override;

	void Bind() const override;
	void Unbind() const override;
	void SetData(const void* data, uint32_t size = 0) override;
private:
	ID3D11Buffer* m_Buffer = nullptr;
};
}
