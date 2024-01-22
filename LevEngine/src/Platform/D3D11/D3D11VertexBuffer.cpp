#include "levpch.h"
#include <wrl/client.h>

#include "D3D11VertexBuffer.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11VertexBuffer::D3D11VertexBuffer(const uint32_t count, const uint32_t stride) : VertexBuffer(count, stride)
{
	D3D11_BUFFER_DESC vertexBufDesc;

	vertexBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = count * stride;

	const auto res = device->CreateBuffer(&vertexBufDesc, nullptr, &m_Buffer);

	LEV_CORE_ASSERT(SUCCEEDED(res), "Unable to create Vertex Buffer")
}

D3D11VertexBuffer::D3D11VertexBuffer(const void* data, uint32_t count, uint32_t stride) : VertexBuffer(count, stride)
{
	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = count * stride;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = data;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	const auto res = device->CreateBuffer(&vertexBufDesc, &vertexData, &m_Buffer);

	LEV_CORE_ASSERT(SUCCEEDED(res), "Unable to create Vertex Buffer")
}

D3D11VertexBuffer::~D3D11VertexBuffer()
{
	if (m_Buffer)
		m_Buffer->Release();
}

void D3D11VertexBuffer::Bind(const uint32_t slot) const
{
	const uint32_t strides[] = { m_Stride };
	constexpr uint32_t offsets[] = { 0 };

	context->IASetVertexBuffers(slot, 1, &m_Buffer, strides, offsets);
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
}
