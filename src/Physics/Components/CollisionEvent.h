#pragma once
#include "Scene/Entity.h"

struct CollisionEvents
{
	entt::delegate<void(Entity, Entity)> onCollisionBegin{};
	entt::delegate<void(Entity, Entity)> onCollisionEnd{};
};
