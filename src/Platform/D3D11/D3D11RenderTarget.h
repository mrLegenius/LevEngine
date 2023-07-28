#pragma once
#include "Renderer/RenderTarget.h"

namespace LevEngine
{
class D3D11RenderTarget final : public RenderTarget
{
public:
    ~D3D11RenderTarget() override = default;

    void Bind() override;
    void Unbind() override;

    bool IsValid() const override;
};
}