#pragma once
#include <d3d11_2.h>

#include "Renderer/Dispatch/DispatchCommands.h"
#include "Renderer/Pipeline/ConstantBuffer.h"

namespace LevEngine
{
    class D3D11DispatchCommand final : public DispatchCommands
    {
    public:
        D3D11DispatchCommand(ID3D11Device2* device);
    protected:
        void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;
        void DispatchIndirect(const Ref<ConstantBuffer>& constantBuffer) const;

        ID3D11DeviceContext2* m_DeviceContext;
    };
}
