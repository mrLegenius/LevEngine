#pragma once
#include "entt/entt.hpp"
#include "..\Components\LegacyRigidbody.h"
namespace LevEngine
{
inline void ForcesClearSystem(const float, entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    const auto view = registry.view<LegacyRigidbody>();

    for (const auto entity : view)
	    view.get<LegacyRigidbody>(entity).ClearForces();
}
}