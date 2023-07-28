#pragma once
#include <d3d11.h>

#include "Renderer/RasterizerState.h"

namespace LevEngine
{

class D3D11RasterizerState final : public RasterizerState
{
public:

    ~D3D11RasterizerState() override;

    void Bind() override;
    void Unbind() override;

private:
    ID3D11RasterizerState* m_RasterizerState = nullptr;
};
}
