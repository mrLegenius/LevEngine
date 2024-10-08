#include "levpch.h"
#include <wrl/client.h>

#include "D3D11IndexBuffer.h"

namespace LevEngine
{
	
D3D11IndexBuffer::D3D11IndexBuffer(ID3D11Device2* device, const uint32_t* indices, const uint32_t count) : IndexBuffer(count)
{
	device->GetImmediateContext2(&m_DeviceContext);
	
	D3D11_BUFFER_DESC indexBufDesc;
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(uint32_t) * count;

	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	const auto res = device->CreateBuffer(&indexBufDesc, &indexData, &m_Buffer);
	LEV_CORE_ASSERT(SUCCEEDED(res), "Unable to create index buffer")
}

D3D11IndexBuffer::~D3D11IndexBuffer()
{
	if (m_Buffer)
		m_Buffer->Release();
}

void D3D11IndexBuffer::Bind() const
{
	m_DeviceContext->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
}

void D3D11IndexBuffer::Unbind() const
{
	
}
}