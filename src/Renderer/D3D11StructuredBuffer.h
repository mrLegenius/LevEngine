#pragma once
#include <wrl/client.h>

#include "CPUAccess.h"
#include "Shader.h"

namespace LevEngine
{
class D3D11StructuredBuffer
{
public:
    enum class UAVType
    {
        None = 0,
        Raw = 1,
        Append = 2, 
        Counter = 4,
    };

    D3D11StructuredBuffer(const void* data, size_t count, uint32_t stride, CPUAccess cpuAccess = CPUAccess::None, bool uav = false, UAVType uavType = UAVType::None);

    ~D3D11StructuredBuffer();

    // Bind the buffer for rendering.
    bool Bind(const unsigned id, const Shader::Type shaderType, const bool readWrite, const uint32_t counterValue = -1);
    // Unbind the buffer for rendering.
    void Unbind(const unsigned id, const Shader::Type shaderType, const bool readWrite) const;

    // How many elements does this buffer contain?
    uint32_t GetElementCount() const { return m_uiCount; }

    void Copy(Ref<D3D11StructuredBuffer> other);

    // Clear the contents of the buffer.
    void Clear() const;

    // Used by the RenderTarget only.
    ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_UnorderedAccessView; }
    uint32_t GetCounterValue() const;

protected:
    void SetData(void* data, size_t elementSize, size_t offset, size_t numElements);
    // Commit the data from system memory to device memory.
    void Commit();

private:;
    ID3D11Buffer* m_Buffer;
    ID3D11Buffer* m_CountBuffer;
    ID3D11ShaderResourceView* m_ShaderResourceView;
    ID3D11UnorderedAccessView* m_UnorderedAccessView;

    // The system data buffer.
    typedef std::vector<uint8_t> BufferType;
    BufferType m_Data;

    // The stride of the vertex buffer in bytes.
    UINT m_uiStride;

    // The number of elements in this buffer.
    UINT m_uiCount;

    // Marked dirty if the contents of the buffer differ
    // from what is stored on the GPU.
    bool m_IsDirty;
    // Does this buffer require GPU write access 
    // If so, it must be bound as a UAV instead of an SRV.
    bool m_UAV;

    // Requires CPU read/write access.
    bool m_Dynamic;
    CPUAccess m_CPUAccess;
};
}
