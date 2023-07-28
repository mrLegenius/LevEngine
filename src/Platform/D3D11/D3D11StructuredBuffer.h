#pragma once
#include <d3d11.h>

#include "Renderer/StructuredBuffer.h"

namespace LevEngine
{

class D3D11StructuredBuffer : public StructuredBuffer
{
public:
    D3D11StructuredBuffer(const void* data, size_t count, uint32_t stride, CPUAccess cpuAccess = CPUAccess::None, bool uav = false, UAVType uavType = UAVType::None);

    ~D3D11StructuredBuffer() override;

    bool Bind(unsigned id, Shader::Type shaderType, bool readWrite, uint32_t counterValue = -1) override;
    void Unbind(unsigned id, Shader::Type shaderType, bool readWrite) const override;

    void Clear() const override;

    void Copy(Ref<D3D11StructuredBuffer> other);

    // Used by the RenderTarget only
    ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_UnorderedAccessView; }
    uint32_t GetCounterValue() const override;

private:
    void SetData(void* data, size_t elementSize, size_t offset, size_t numElements);
    // Commit the data from system memory to device memory
    void Commit();

    ID3D11Buffer* m_Buffer;
    ID3D11Buffer* m_CountBuffer;
    ID3D11ShaderResourceView* m_ShaderResourceView;
    ID3D11UnorderedAccessView* m_UnorderedAccessView;
};
}
