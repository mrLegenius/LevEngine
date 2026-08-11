#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class LEV_API ParticlesUtils
    {
    public:
        static void GetGroupSize(uint32_t totalCount, int& groupSizeX, int& groupSizeY);
    private:
        static constexpr uint32_t c_MaxThreadCount = 1024;
    };
}