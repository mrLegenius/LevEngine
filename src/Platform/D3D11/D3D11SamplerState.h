#pragma once
#include <d3d11.h>
#include "Renderer/SamplerState.h"

namespace LevEngine
{
    class D3D11SamplerState final : public SamplerState
    {
    public:
        D3D11SamplerState() = default;
        ~D3D11SamplerState() override;

        void Bind(uint32_t slot, Shader::Type shaderType) override;
        void UnBind(uint32_t slot, Shader::Type shaderType) override;

    protected:
        [[nodiscard]] D3D11_FILTER TranslateFilter() const;
        [[nodiscard]] D3D11_TEXTURE_ADDRESS_MODE TranslateWrapMode(WrapMode wrapMode) const;
        [[nodiscard]] D3D11_COMPARISON_FUNC TranslateComparisonFunction(CompareFunc compareFunc) const;

    private:

        ID3D11SamplerState* m_SamplerState;
    };
}
