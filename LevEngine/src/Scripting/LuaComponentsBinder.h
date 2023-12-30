#pragma once
#include <sol/sol.hpp>

namespace sol {
	template <typename T>
	struct unique_usertype_traits<LevEngine::Ref<T>> {
		typedef T type;
		typedef LevEngine::Ref<T> actual_type;
		static const bool value = true;

		static bool is_null(const actual_type& ptr) {
			return ptr == nullptr;
		}

		static type* get (const actual_type& ptr) {
			return ptr.get();
		}
	};

	template <class T>
	struct is_container<Vector<T>> : std::true_type { };
}

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
		static void CreateSceneManagerBind(sol::state& lua);
		static void CreateSceneBind(sol::state& lua);
		static void CreatePrefabBind(sol::state& lua);
		static void CreateAudioBind(sol::state& lua);
		
		static void CreateTransformLuaBind(sol::state& lua);
		static void CreateCameraComponentLuaBind(sol::state& lua);
		static void CreateScriptsContainerLuaBind(sol::state& lua);
		static void CreateRigidbodyLuaBind(sol::state& lua);
		
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

	template <class TComponent>
	auto get_or_add_component(Entity& entity, sol::this_state state);
}

#include "LuaComponentsBinder.inl"
