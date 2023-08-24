#include "levpch.h"
#include <wrl/client.h>

#include "D3D11ConstantBuffer.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11ConstantBuffer::D3D11ConstantBuffer(const uint32_t size, const uint32_t slot) : ConstantBuffer(size, slot)
{
	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;
	bufferDesc.ByteWidth = size;

	const auto result = device->CreateBuffer(&bufferDesc, nullptr, &m_Buffer);

	LEV_CORE_ASSERT(SUCCEEDED(result), "Unable to create constant buffer")
}

D3D11ConstantBuffer::~D3D11ConstantBuffer()
{
	m_Buffer->Release();
}

void D3D11ConstantBuffer::SetData(const void* data, const uint32_t size) const
{
	LEV_CORE_ASSERT(m_Buffer != nullptr, "Trying to SetData with null buffer");
		
	const auto actualSize = size ? size : m_Size;
	D3D11_MAPPED_SUBRESOURCE resource;
	context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, data, actualSize);
	context->Unmap(m_Buffer, 0);
}

void D3D11ConstantBuffer::Bind(const Shader::Type shaderType)
{
	if (shaderType & Shader::Type::Vertex)
		context->VSSetConstantBuffers(m_Slot, 1, &m_Buffer);
	if (shaderType & Shader::Type::Pixel)
		context->PSSetConstantBuffers(m_Slot, 1, &m_Buffer);
	if (shaderType & Shader::Type::Geometry)
		context->GSSetConstantBuffers(m_Slot, 1, &m_Buffer);
	if (shaderType & Shader::Type::Compute)
		context->CSSetConstantBuffers(m_Slot, 1, &m_Buffer);
}
}
