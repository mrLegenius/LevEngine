#pragma once
#include "Renderer/DispatchCommandBase.h"

namespace LevEngine
{
    class D3D11DispatchCommand final : public DispatchCommandBase
    {
    protected:
        void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ) override;
    };
}
