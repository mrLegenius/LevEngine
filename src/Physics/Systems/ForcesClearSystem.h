#pragma once
#include "entt/entt.hpp"
#include "Physics/Components/Rigidbody.h"

inline void ForcesClearSystem(const float, entt::registry& registry)
{
    LEV_PROFILE_FUNCTION();

    const auto view = registry.view<Rigidbody>();

    for (const auto entity : view)
	    view.get<Rigidbody>(entity).ClearForces();
}
