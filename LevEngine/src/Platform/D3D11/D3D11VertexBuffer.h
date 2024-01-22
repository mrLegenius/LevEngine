#pragma once
#include <d3d11.h>

#include "Renderer/VertexBuffer.h"

namespace LevEngine
{
class D3D11VertexBuffer final : public VertexBuffer
{
public:
	D3D11VertexBuffer(uint32_t count, uint32_t stride);
	
	template<typename T>
	D3D11VertexBuffer(const T* data, uint32_t count, uint32_t stride);
	
	~D3D11VertexBuffer() override;

	void Bind(uint32_t slot = 0) const override;
	void Unbind() const override;
	void SetData(const void* data, uint32_t size = 0) override;
private:
	D3D11VertexBuffer(const void* data, uint32_t count, uint32_t stride);
	
	ID3D11Buffer* m_Buffer = nullptr;
};
}

#include "D3D11VertexBuffer.inl"