#pragma once
#include <d3d11_2.h>

#include "Renderer/Pipeline/StructuredBuffer.h"

namespace LevEngine
{
    enum class ShaderType;

    class D3D11StructuredBuffer : public StructuredBuffer
    {
    public:
        D3D11StructuredBuffer(ID3D11Device2* device, const void* data, size_t count, uint32_t stride, CPUAccess cpuAccess = CPUAccess::None,
                              bool uav = false, UAVType uavType = UAVType::None);

        ~D3D11StructuredBuffer() override;

        bool Bind(uint32_t slot, ShaderType shaderType, bool readWrite, uint32_t counterValue = -1) override;
        void Unbind(uint32_t slot, ShaderType shaderType, bool readWrite) const override;

        void Clear() const override;

        void Copy(const Ref<D3D11StructuredBuffer>& other);
        
        void BindCounter(uint32_t slot, ShaderType shaderType) override;
        void UnbindCounter(uint32_t slot, ShaderType shaderType) override;
        uint32_t GetCounterValue() const override;
        
        // Used by the RenderTarget only
        ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_UnorderedAccessView; }


    private:
        void SetData(void* data, size_t elementSize, size_t offset, size_t numElements);
        // Commit the data from system memory to device memory
        void Commit();
        
        ID3D11Buffer* m_Buffer;
        ID3D11Buffer* m_CountBuffer;
        ID3D11ShaderResourceView* m_ShaderResourceView;
        ID3D11UnorderedAccessView* m_UnorderedAccessView;

        ID3D11DeviceContext2* m_DeviceContext;
    };
}
