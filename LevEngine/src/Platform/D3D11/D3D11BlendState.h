#pragma once
#include <d3d11.h>

#include "Renderer/BlendState.h"

namespace LevEngine
{
class D3D11BlendState final : public BlendState
{
public:
    D3D11BlendState() = default;
	~D3D11BlendState() override;
    void Bind() override;
    void Unbind() override;

private:
    ID3D11BlendState* m_BlendState = nullptr;
};
}
