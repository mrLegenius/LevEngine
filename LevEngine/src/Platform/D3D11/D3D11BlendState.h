#pragma once
#include <d3d11_2.h>

#include "Renderer/Pipeline/BlendState.h"

namespace LevEngine
{
class D3D11BlendState final : public BlendState
{
public:
    D3D11BlendState(ID3D11Device2* device);
	~D3D11BlendState() override;
    void Bind() override;
    void Unbind() override;

private:
    ID3D11BlendState* m_BlendState{};

    ID3D11Device2* m_Device{};
    ID3D11DeviceContext2* m_DeviceContext{};
};
}
