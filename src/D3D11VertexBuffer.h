#pragma once
#include <cstdint>

#include "Buffer.h"
#include "D3D11Shader.h"

class D3D11VertexBuffer final
{
public:
	explicit D3D11VertexBuffer(uint32_t size);
	D3D11VertexBuffer(float* vertices, uint32_t size);
	~D3D11VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetLayout(const BufferLayout& layout) { m_Layout = layout;}
	[[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }

	[[nodiscard]] uint32_t GetSize() const { return m_Size; }

	void SetData(const void* data, uint32_t size = 0);
private:
	ID3D11Buffer* m_Buffer = nullptr;
	uint32_t m_Size;
	BufferLayout m_Layout;
};
