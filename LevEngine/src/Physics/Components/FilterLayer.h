#pragma once
#include "Essentials/EnumUtils.h"

namespace LevEngine
{
    enum class FilterLayer : uint32_t
    {
        None   = 0,
        Layer0 = (1 << 0),
        Layer1 = (1 << 1),
        Layer2 = (1 << 2),
        Layer3 = (1 << 3),
        Layer4 = (1 << 4),
        Layer5 = (1 << 5),
        Layer6 = (1 << 6),
        Layer7 = (1 << 7),
        Layer8 = (1 << 8),
        Layer9 = (1 << 9),
        All    = 0xFFFFFFFF
    };
    ENUM_AS_FLAG(FilterLayer)
}