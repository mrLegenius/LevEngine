#pragma once
#include <cstdint>

#include "D3D11VertexBuffer.h"

class D3D11IndexBuffer final
{
public:
	D3D11IndexBuffer(uint32_t* indices, uint32_t count);
	~D3D11IndexBuffer();

	void Bind() const;
	void Unbind() const;

	[[nodiscard]] uint32_t GetCount() const { return m_Count; }

private:
	ID3D11Buffer* m_Buffer = nullptr;
	uint32_t m_Count = 0;
};

