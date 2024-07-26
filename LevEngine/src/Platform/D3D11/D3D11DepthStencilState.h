#pragma once
#include <d3d11.h>
#include "Renderer/Pipeline/DepthStencilState.h"

namespace LevEngine
{
class D3D11DepthStencilState final : public DepthStencilState
{
public:
    ~D3D11DepthStencilState() override;

    void Bind() override;
    void Unbind() override;

private:
    ID3D11DepthStencilState* m_DepthStencilState = nullptr;
};
}
