#include "D3D11ConstantBuffer.h"

#include <wrl/client.h>

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11ConstantBuffer::D3D11ConstantBuffer(uint32_t size, uint32_t slot) : m_Size(size), m_Slot(slot)
{
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.ByteWidth = size;

	device->CreateBuffer(&bufferDesc, nullptr, &m_Buffer);
}

D3D11ConstantBuffer::~D3D11ConstantBuffer()
{
	m_Buffer->Release();
}

void D3D11ConstantBuffer::SetData(const void* data, uint32_t size, uint32_t offset) const
{
	const auto actualSize = size ? size : m_Size;
	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, actualSize);
	context->Unmap(m_Buffer, 0);

	context->VSSetConstantBuffers(m_Slot, 1, &m_Buffer);
	context->PSSetConstantBuffers(m_Slot, 1, &m_Buffer);
}
