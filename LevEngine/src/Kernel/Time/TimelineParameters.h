#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    struct LEV_API TimelineParameters
    {
        bool isLooping{};
        double duration{};
        double timeScale{1.0};
    };
}