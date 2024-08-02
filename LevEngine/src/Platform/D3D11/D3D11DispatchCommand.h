#pragma once
#include "Renderer/Dispatch/DispatchCommands.h"

namespace LevEngine
{
    class D3D11DispatchCommand final : public DispatchCommands
    {
    protected:
        void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;
    };
}
