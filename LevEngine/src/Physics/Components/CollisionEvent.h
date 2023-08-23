#pragma once
#include "Scene/Entity.h"
namespace LevEngine
{
struct CollisionEvents
{
	entt::delegate<void(Entity, Entity)> onCollisionBegin{};
	entt::delegate<void(Entity, Entity)> onCollisionEnd{};
};
}