#pragma once
#include <d3d11_2.h>

#include "Renderer/Dispatch/DispatchCommands.h"

namespace LevEngine
{
    class D3D11DispatchCommand final : public DispatchCommands
    {
    public:
        D3D11DispatchCommand(ID3D11Device2* device);
    protected:
        void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;

        ID3D11DeviceContext2* m_DeviceContext;
    };
}
