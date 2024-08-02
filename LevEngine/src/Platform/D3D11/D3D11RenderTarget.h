#pragma once
#include <d3d11_2.h>

#include "Renderer/Pipeline/RenderTarget.h"

namespace LevEngine
{
class D3D11RenderTarget final : public RenderTarget
{
public:
    D3D11RenderTarget(ID3D11Device2* device);
    ~D3D11RenderTarget() override = default;

    void Bind() override;
    void Unbind() override;

    bool IsValid() const override;

private:
    ID3D11DeviceContext2* m_DeviceContext;
};
}