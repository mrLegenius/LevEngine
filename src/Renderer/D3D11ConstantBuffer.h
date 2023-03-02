#pragma once
#include <cstdint>
#include <d3d11.h>

class D3D11ConstantBuffer
{
public:
	explicit D3D11ConstantBuffer(uint32_t size, uint32_t slot = 0);
	~D3D11ConstantBuffer();
	void SetData(const void* data, uint32_t size, uint32_t offset = 0) const;
private:
	ID3D11Buffer* m_Buffer = nullptr;
	uint32_t m_Size;
	uint32_t m_Slot;
};
