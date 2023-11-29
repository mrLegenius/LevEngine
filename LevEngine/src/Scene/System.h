#pragma once
namespace LevEngine
{
using namespace entt::literals;
class System
{
public:
	friend class Scene;
	virtual ~System() = default;
	virtual void Update(float deltaTime, entt::registry& registry) = 0;
};
}