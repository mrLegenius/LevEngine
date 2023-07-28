#include "pch.h"
#include <d3d11.h>
#include <wrl/client.h>

#include "D3D11StructuredBuffer.h"

namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

D3D11_BUFFER_UAV_FLAG ConvertUAVFlag(D3D11StructuredBuffer::UAVType type)
{
    return static_cast<D3D11_BUFFER_UAV_FLAG>(type);
}

D3D11StructuredBuffer::D3D11StructuredBuffer(const void* data, size_t count, uint32_t stride,
    CPUAccess cpuAccess, bool uav, UAVType uavType) : StructuredBuffer(count, stride, cpuAccess)
	{
    m_Dynamic = (int)m_CPUAccess & (int)CPUAccess::Write;
    m_UAV = uav && !m_Dynamic;

    // Assign the data to the system buffer.
    size_t numBytes = m_ElementsCount * m_Stride;

    if (data)
    {
        m_Data.assign((uint8_t*)data, (uint8_t*)data + numBytes);
    }
    else
    {
        m_Data.reserve(numBytes);
    }

    // Create a GPU buffer to store the data.
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = (UINT)numBytes;

    if (((int)m_CPUAccess & (int)CPUAccess::ReadWrite) != 0)
    {
        bufferDesc.Usage = D3D11_USAGE_STAGING;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    }
    else if (((int)m_CPUAccess & (int)CPUAccess::Read) != 0)
    {
        bufferDesc.Usage = D3D11_USAGE_STAGING;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    }
    else if (((int)m_CPUAccess & (int)CPUAccess::Write) != 0)
    {
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    }
    else
    {
        bufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        if (m_UAV)
	        bufferDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    }

    bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bufferDesc.StructureByteStride = m_Stride;

    D3D11_SUBRESOURCE_DATA subResourceData;
    subResourceData.pSysMem = (void*)m_Data.data();
    subResourceData.SysMemPitch = 0;
    subResourceData.SysMemSlicePitch = 0;

    auto res = device->CreateBuffer(&bufferDesc, &subResourceData, &m_Buffer);

    LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create read/write buffer")

    if ((bufferDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = m_ElementsCount;

        res = device->CreateShaderResourceView(m_Buffer, &srvDesc, &m_ShaderResourceView);

        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create shader resource view")
    }

    if ((bufferDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0)
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        uavDesc.Format = DXGI_FORMAT_UNKNOWN;
        uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        uavDesc.Buffer.FirstElement = 0;
        uavDesc.Buffer.NumElements = m_ElementsCount;
        uavDesc.Buffer.Flags = ConvertUAVFlag(uavType);

        res = device->CreateUnorderedAccessView(m_Buffer, &uavDesc, &m_UnorderedAccessView);
        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create unordered access view")

        if (uavType == UAVType::Append || uavType == UAVType::Counter)
        {
            D3D11_BUFFER_DESC countBufferDesc{};
            countBufferDesc.BindFlags = 0;
            countBufferDesc.Usage = D3D11_USAGE_STAGING;
            countBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            countBufferDesc.MiscFlags = 0;
            countBufferDesc.StructureByteStride = 0;
            countBufferDesc.ByteWidth = 4;

            res = device->CreateBuffer(&countBufferDesc, nullptr, &m_CountBuffer);
            LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to create count buffer")
        }
    }
}

D3D11StructuredBuffer::~D3D11StructuredBuffer()
{
    m_Buffer->Release();

    if (m_ShaderResourceView)
		m_ShaderResourceView->Release();
    if (m_UnorderedAccessView)
		m_UnorderedAccessView->Release();
}

bool D3D11StructuredBuffer::Bind(const unsigned id, const Shader::Type shaderType, const bool readWrite, const uint32_t counterValue)
{
    if (m_IsDirty)
    {
        Commit();
        m_IsDirty = false;
    }

    if (!readWrite && m_ShaderResourceView)
    {
        ID3D11ShaderResourceView* srv[] = { m_ShaderResourceView };

        switch (shaderType)
        {
        case Shader::Type::Vertex:
            context->VSSetShaderResources(id, 1, srv);
            break;
        case Shader::Type::Geometry:
            context->GSSetShaderResources(id, 1, srv);
            break;
        case Shader::Type::Pixel:
            context->PSSetShaderResources(id, 1, srv);
            break;
        case Shader::Type::Compute:
            context->CSSetShaderResources(id, 1, srv);
            break;
        }
    }
    else if (readWrite && m_UnorderedAccessView && m_UAV)
    {
        ID3D11UnorderedAccessView* uav[] = { m_UnorderedAccessView };
        switch (shaderType)
        {
        case Shader::Type::Compute:
	        const auto counter = counterValue;
            context->CSSetUnorderedAccessViews(id, 1, uav, &counter);
            break;
        }
    }

    return true;
}

void D3D11StructuredBuffer::Unbind(const unsigned id, const Shader::Type shaderType, const bool readWrite) const
{
    ID3D11UnorderedAccessView* uav[] = { nullptr };
    ID3D11ShaderResourceView* srv[] = { nullptr };

    if (!readWrite && m_ShaderResourceView)
    {
        switch (shaderType)
        {
        case Shader::Type::Vertex:
            context->VSSetShaderResources(id, 1, srv);
            break;
        case Shader::Type::Geometry:
            context->GSSetShaderResources(id, 1, srv);
            break;
        case Shader::Type::Pixel:
            context->PSSetShaderResources(id, 1, srv);
            break;
        case Shader::Type::Compute:
            context->CSSetShaderResources(id, 1, srv);
            break;
        }
    }
    else if (readWrite && m_UnorderedAccessView)
    {
        switch (shaderType)
        {
        case Shader::Type::Compute:
            uint32_t counterZero = 0;
            context->CSSetUnorderedAccessViews(id, 1, uav, &counterZero);
            break;
        }
    }

}

void D3D11StructuredBuffer::Copy(const Ref<D3D11StructuredBuffer> other)
{
	const std::shared_ptr<D3D11StructuredBuffer> srcBuffer = other;

    if (srcBuffer->m_IsDirty)
    {
        // Make sure the contents of the source buffer are up-to-date
        srcBuffer->Commit();
    }

    if (srcBuffer && srcBuffer.get() != this &&
        m_ElementsCount * m_Stride == srcBuffer->m_ElementsCount * srcBuffer->m_Stride)
    {
        context->CopyResource(m_Buffer, srcBuffer->m_Buffer);
    }
    else
    {
        LEV_THROW("Source buffer is not compatible with this buffer")
    }

    if (((int)m_CPUAccess & (int)CPUAccess::Read) != 0)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        // Copy the texture data from the buffer resource
        auto res = context->Map(m_Buffer, 0, D3D11_MAP_READ, 0, &mappedResource);

        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to map texture resource for reading")

        memcpy_s(m_Data.data(), m_Data.size(), mappedResource.pData, m_Data.size());

        context->Unmap(m_Buffer, 0);
    }
}

void D3D11StructuredBuffer::Clear() const
{
    if (m_UAV)
    {
	    constexpr float clearColor[4] = { 0, 0, 0, 0 };
        context->ClearUnorderedAccessViewFloat(m_UnorderedAccessView, clearColor);
    }
}

void D3D11StructuredBuffer::SetData(void* data, size_t elementSize, size_t offset, size_t numElements)
{
    unsigned char* first = (unsigned char*)data + (offset * elementSize);
    unsigned char* last = first + (numElements * elementSize);
    m_Data.assign(first, last);

    m_IsDirty = true;
}

void D3D11StructuredBuffer::Commit()
{
    if (m_IsDirty && m_Dynamic && m_Buffer)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        // Copy the contents of the data buffer to the GPU.

        auto res = context->Map(m_Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        LEV_CORE_ASSERT(SUCCEEDED(res), "Failed to map subresource")

        size_t sizeInBytes = m_Data.size();
        memcpy_s(mappedResource.pData, sizeInBytes, m_Data.data(), sizeInBytes);

        context->Unmap(m_Buffer, 0);

        m_IsDirty = false;
    }
}

uint32_t D3D11StructuredBuffer::GetCounterValue() const
{
    LEV_CORE_ASSERT(m_CountBuffer != nullptr, "Count buffer is null");
    LEV_CORE_ASSERT(m_UnorderedAccessView != nullptr, "UnorderedAccessView is null");

    context->CopyStructureCount(m_CountBuffer, 0, m_UnorderedAccessView);

    D3D11_MAPPED_SUBRESOURCE subresource;
    context->Map(m_CountBuffer, 0, D3D11_MAP_READ, 0, &subresource);

    const UINT* data = reinterpret_cast<UINT*>(subresource.pData);
    const uint32_t counterValue = data[0];

    context->Unmap(m_CountBuffer, 0);

    return counterValue;
}
}