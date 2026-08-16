#pragma once

#include "Kernel/Core.h"
#include <d3d11_2.h>

#include "Renderer/Pipeline/StructuredBuffer.h"

namespace LevEngine
{
    enum class ShaderType;

    class LEV_API D3D11StructuredBuffer : public StructuredBuffer
    {
    public:
        D3D11StructuredBuffer(ID3D11Device2* device, const void* data, size_t count, uint32_t stride, CPUAccess cpuAccess = CPUAccess::None,
                              bool uav = false, UAVType uavType = UAVType::None);

        ~D3D11StructuredBuffer() override;

        bool Bind(uint32_t slot, ShaderType shaderType, bool readWrite, uint32_t counterValue = -1) override;
        void Unbind(uint32_t slot, ShaderType shaderType, bool readWrite) const override;

        void Clear() const override;

        void SetData(const void* data, size_t elementSize, size_t offset, size_t numElements) override;

        void Copy(const Ref<D3D11StructuredBuffer>& other);
        
        void BindCounter(uint32_t slot, ShaderType shaderType) override;
        void UnbindCounter(uint32_t slot, ShaderType shaderType) override;
        uint32_t GetCounterValue() const override;
        
        // Used by the RenderTarget only
        ID3D11UnorderedAccessView* GetUnorderedAccessView() const { return m_UnorderedAccessView; }


    private:
        // Commit the data from system memory to device memory
        void Commit();
        
        // Null by default, and that is not decoration. Which of these the constructor fills depends on
        // the bind flags: a dynamic buffer gets a shader resource view and no unordered access view, a
        // staging one gets neither, and only an Append or Counter UAV gets a count buffer. The
        // destructor guards each with a null test, so leaving them uninitialized meant testing stack
        // garbage -- which passes, and then releases a wild pointer. That was the segfault on every
        // exit: Renderer3D's instance buffer is dynamic, so its unordered access view was never
        // assigned and never null.
        ID3D11Buffer* m_Buffer = nullptr;
        ID3D11Buffer* m_CountBuffer = nullptr;
        ID3D11ShaderResourceView* m_ShaderResourceView = nullptr;
        ID3D11UnorderedAccessView* m_UnorderedAccessView = nullptr;

        ID3D11DeviceContext2* m_DeviceContext = nullptr;
    };
}
