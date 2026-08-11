#pragma once

#include "Kernel/Core.h"
namespace LevEngine
{
using namespace entt::literals;
class LEV_API System
{
public:
	friend class Scene;
	virtual ~System() = default;
	virtual void Update(float deltaTime, entt::registry& registry) = 0;
};
}