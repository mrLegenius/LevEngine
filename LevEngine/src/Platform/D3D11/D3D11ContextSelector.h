#pragma once
#include "D3D11RendererContext.h"

namespace LevEngine
{
    class D3D11ContextSelector
    {
    public:
        static ID3D11DeviceContext* GetContext();
    };
}

