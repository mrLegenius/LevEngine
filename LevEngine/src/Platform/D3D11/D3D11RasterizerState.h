#pragma once
#include <d3d11_2.h>

#include "Renderer/Pipeline/RasterizerState.h"

namespace LevEngine
{

class D3D11RasterizerState final : public RasterizerState
{
public:
    D3D11RasterizerState(ID3D11Device2* device);
    ~D3D11RasterizerState() override;

    void Bind() override;
    void Unbind() override;

private:
    ID3D11RasterizerState* m_RasterizerState = nullptr;
    Vector<D3D11_RECT> m_d3dRects;
    Vector<D3D11_VIEWPORT> m_d3dViewports;

    ID3D11Device2* m_Device;
    ID3D11DeviceContext2* m_DeviceContext;
};
}
