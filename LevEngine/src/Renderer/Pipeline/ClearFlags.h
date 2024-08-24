#pragma once
#include <cstdint>

#include "Essentials/EnumUtils.h"

namespace LevEngine
{
    enum class ClearFlags : uint8_t
    {
        Color = 1 << 0,
        Depth = 1 << 1,
        Stencil = 1 << 2,
        DepthStencil = Depth | Stencil,
        All = Color | Depth | Stencil,
    };

    ENUM_AS_FLAG(ClearFlags)
}
