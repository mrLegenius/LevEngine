#include "D3D11IndexBuffer.h"
#include <wrl/client.h>
#include "D3D11Shader.h"

extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11IndexBuffer::D3D11IndexBuffer(uint32_t* indices, const uint32_t count)
	: m_Count(count)
{
	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(uint32_t) * count;

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&indexBufDesc, &indexData, &m_Buffer);
}

D3D11IndexBuffer::~D3D11IndexBuffer()
{
	m_Buffer->Release();
}

void D3D11IndexBuffer::Bind() const
{
	context->IASetIndexBuffer(m_Buffer, DXGI_FORMAT_R32_UINT, 0);
}

void D3D11IndexBuffer::Unbind() const
{
	
}