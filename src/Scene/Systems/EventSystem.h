#pragma once
#include "../System.h"
namespace LevEngine
{
template<typename T>
class EventSystem final : public System
{
	void Update(float deltaTime, entt::registry& registry) override
	{
		registry.clear<T>();
	}
};
}
