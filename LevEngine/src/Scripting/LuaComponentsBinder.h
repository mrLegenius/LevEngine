#pragma once
#include <sol/sol.hpp>

namespace LevEngine
{
	class Entity;
	class Scene;
}

namespace LevEngine::Scripting
{
	class LuaComponentsBinder
	{
	public:
		static void CreateInputLuaBind(sol::state& lua);
		static void CreateTransformLuaBind(sol::state& lua);
		static void CreateCameraComponentLuaBind(sol::state& lua);
		static void CreateLuaEntityBind(sol::state& lua, Scene* scene);

		template<class TComponent>
		static void RegisterMetaComponent();
	};

	template <class TComponent>
	auto add_component(Entity& entity, const sol::table& component, sol::this_state state);

	template <class TComponent>
	auto has_component(Entity& entity);

	template <class TComponent>
	auto get_component(Entity& entity, sol::this_state state);

	template <class TComponent>
	auto remove_component(Entity& entity);
}

#include "LuaComponentsBinder.inl"
