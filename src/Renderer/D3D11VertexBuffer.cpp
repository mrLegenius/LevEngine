#include "D3D11VertexBuffer.h"

#include <directxmath.h>
#include <wrl/client.h>

#include "D3D11Shader.h"

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11VertexBuffer::D3D11VertexBuffer(const uint32_t size) : m_Size(size)
{
	D3D11_BUFFER_DESC vertexBufDesc;

	vertexBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = size;

	device->CreateBuffer(&vertexBufDesc, nullptr, &m_Buffer);
}

D3D11VertexBuffer::D3D11VertexBuffer(float* vertices, const uint32_t size) : m_Size(size)
{
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = size;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&vertexBufDesc, &vertexData, &m_Buffer);
}

D3D11VertexBuffer::~D3D11VertexBuffer()
{
	m_Buffer->Release();
}

void D3D11VertexBuffer::Bind() const
{
	const UINT strides[] = { m_Layout.GetStride() };
	constexpr UINT offsets[] = { 0 };

	context->IASetVertexBuffers(0, 1, &m_Buffer, strides, offsets);
}

void D3D11VertexBuffer::Unbind() const
{
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
}

void D3D11VertexBuffer::SetData(const void* data, const uint32_t size)
{
	const auto actualSize = size ? size : m_Size;
	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, actualSize);
	context->Unmap(m_Buffer, 0);
}
