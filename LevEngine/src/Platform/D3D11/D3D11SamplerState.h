#pragma once
#include <d3d11_2.h>

#include "Renderer/Pipeline/SamplerState.h"

namespace LevEngine
{
    enum class ShaderType;

    class D3D11SamplerState final : public SamplerState
    {
    public:
        D3D11SamplerState(ID3D11Device2* device);
        ~D3D11SamplerState() override;

        void Bind(uint32_t slot, ShaderType shaderType) override;
        void Unbind(uint32_t slot, ShaderType shaderType) override;

        [[nodiscard]] ID3D11SamplerState* GetSamplerState() const
        {
            return m_SamplerState;
        }

    protected:
        [[nodiscard]] D3D11_FILTER TranslateFilter() const;
        [[nodiscard]] D3D11_TEXTURE_ADDRESS_MODE TranslateWrapMode(WrapMode wrapMode) const;
        [[nodiscard]] D3D11_COMPARISON_FUNC TranslateComparisonFunction(CompareFunc compareFunc) const;

    private:
        ID3D11SamplerState* m_SamplerState{};
        ID3D11Device2* m_Device{};
        ID3D11DeviceContext2* m_DeviceContext{};
    };
}
