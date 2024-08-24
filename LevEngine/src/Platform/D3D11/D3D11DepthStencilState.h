#pragma once
#include <d3d11_2.h>

#include "Renderer/Pipeline/DepthStencilState.h"

namespace LevEngine
{
class D3D11DepthStencilState final : public DepthStencilState
{
public:
    D3D11DepthStencilState(ID3D11Device2* device);
    ~D3D11DepthStencilState() override;

    void Bind() override;
    void Unbind() override;

private:
    ID3D11DepthStencilState* m_DepthStencilState = nullptr;

    ID3D11Device2* m_Device;
    ID3D11DeviceContext2* m_DeviceContext;
};
}
