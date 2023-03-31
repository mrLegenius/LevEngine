#pragma once
#include "../System.h"

template<typename T>
class EventSystem final : public System
{
	void Update(float deltaTime, entt::registry& registry) override
	{
		registry.clear<T>();
	}
};
