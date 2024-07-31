#pragma once
#include <d3d11.h>

#include "Renderer/Pipeline/ConstantBuffer.h"

namespace LevEngine
{
    enum class ShaderType;

    class D3D11ConstantBuffer final : public ConstantBuffer
    {
    public:
        explicit D3D11ConstantBuffer(uint32_t size, uint32_t slot = 0);
        ~D3D11ConstantBuffer() override;

        void SetData(const void* data, uint32_t size = 0) const override;
        void Bind(ShaderType shaderType) override;
        void Bind(uint32_t slot, ShaderType shaderType) override;
        void Unbind(ShaderType shaderType) override;
        void Unbind(uint32_t slot, ShaderType shaderType) override;

    private:
        ID3D11Buffer* m_Buffer = nullptr;
    };
}
