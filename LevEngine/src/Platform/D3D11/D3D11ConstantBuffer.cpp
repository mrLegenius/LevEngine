#include "levpch.h"

#include "D3D11ConstantBuffer.h"

#include "Renderer/Shader/ShaderType.h"

namespace LevEngine
{
    D3D11ConstantBuffer::D3D11ConstantBuffer(ID3D11Device2* device, const uint32_t size, const uint32_t slot) : ConstantBuffer(size, slot)
    {
        device->GetImmediateContext2(&m_Context);
        
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
        m_Context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
        memcpy(resource.pData, data, actualSize);
        m_Context->Unmap(m_Buffer, 0);
    }

    void D3D11ConstantBuffer::Bind(const ShaderType shaderType)
    {
        Bind(m_Slot, shaderType);
    }

    void D3D11ConstantBuffer::Bind(const uint32_t slot, const ShaderType shaderType)
    {
        if (shaderType & ShaderType::Vertex)
            m_Context->VSSetConstantBuffers(slot, 1, &m_Buffer);
        if (shaderType & ShaderType::Pixel)
            m_Context->PSSetConstantBuffers(slot, 1, &m_Buffer);
        if (shaderType & ShaderType::Geometry)
            m_Context->GSSetConstantBuffers(slot, 1, &m_Buffer);
        if (shaderType & ShaderType::Compute)
            m_Context->CSSetConstantBuffers(slot, 1, &m_Buffer);
    }

    void D3D11ConstantBuffer::Unbind(ShaderType shaderType)
    {
        Unbind(m_Slot, shaderType);
    }

    void D3D11ConstantBuffer::Unbind(uint32_t slot, const ShaderType shaderType)
    {
        if (shaderType & ShaderType::Vertex)
            m_Context->VSSetConstantBuffers(slot, 0, nullptr);
        if (shaderType & ShaderType::Pixel)
            m_Context->PSSetConstantBuffers(slot, 0, nullptr);
        if (shaderType & ShaderType::Geometry)
            m_Context->GSSetConstantBuffers(slot, 0, nullptr);
        if (shaderType & ShaderType::Compute)
            m_Context->CSSetConstantBuffers(slot, 0, nullptr);
    }

    ID3D11Buffer* D3D11ConstantBuffer::GetBuffer() const { return m_Buffer; }
}
