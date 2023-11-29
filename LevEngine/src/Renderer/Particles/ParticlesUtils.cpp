#include "levpch.h"
#include "ParticlesUtils.h"

namespace LevEngine
{
    void ParticlesUtils::GetGroupSize(const uint32_t totalCount, int& groupSizeX, int& groupSizeY)
    {
        LEV_PROFILE_FUNCTION();

        const uint32_t numGroups = (totalCount % c_MaxThreadCount != 0)
                                       ? ((totalCount / c_MaxThreadCount) + 1)
                                       : (totalCount / c_MaxThreadCount);
        const double secondRoot = std::ceil(std::pow(static_cast<double>(numGroups), 0.5));
        groupSizeX = static_cast<int>(secondRoot);
        groupSizeY = static_cast<int>(secondRoot);
    }
}
