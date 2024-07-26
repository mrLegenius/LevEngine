#pragma once
#include <d3d11.h>

#include "Renderer/Pipeline/RasterizerState.h"

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
    Vector<D3D11_RECT> m_d3dRects;
    Vector<D3D11_VIEWPORT> m_d3dViewports;
};
}
