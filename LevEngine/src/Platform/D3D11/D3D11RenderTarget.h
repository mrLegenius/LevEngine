#pragma once
#include "Renderer/Pipeline/RenderTarget.h"

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