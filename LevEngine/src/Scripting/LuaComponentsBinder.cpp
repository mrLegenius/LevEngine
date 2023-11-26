#include "levpch.h"
#include "LuaComponentsBinder.h"

#include <entt/entt.hpp>

#include "MetaUtilities.h"
#include "Input/Input.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine::Scripting
{
	void LuaComponentsBinder::CreateInputLuaBind(sol::state& lua)
	{
		lua.new_usertype<Input>(
			"Input",
			sol::no_constructor,
			"getMouseDelta", []()
			{
				return Input::GetMouseDelta();
			}
		);
	}

	void LuaComponentsBinder::CreateTransformLuaBind(sol::state& lua)
	{
		lua.new_usertype <Transform>(
			"Transform",
			"type_id", &entt::type_hash<Transform>::value,
			sol::call_constructor,
			sol::factories(
				[]()
				{
					return Transform();
				},
				[](const Entity entity)
				{
					return Transform(entity);
				}),
			//Position
			"getLocalPosition", [](const Transform& transform)
			{
				return transform.GetLocalPosition();
			},
			"setLocalPosition", [](Transform& transform, const Vector3& position)
			{
				return transform.SetLocalPosition(position);
			},
			"getWorldPosition", [](const Transform& transform)
			{
				return transform.GetWorldPosition();
			},
			"setWorldPosition", [](Transform& transform, const Vector3& position)
			{
				return transform.SetWorldPosition(position);
			},
			//Scale
			"getLocalScale", [](const Transform& transform)
			{
				return transform.GetLocalScale();
			},
			"setLocalScale", [](Transform& transform, const Vector3& scale)
			{
				return transform.SetLocalScale(scale);
			},
			"getWorldScale", [](const Transform& transform)
			{
				return transform.GetWorldScale();
			},
			"setWorldScale", [](Transform& transform, const Vector3& scale)
			{
				return transform.SetWorldScale(scale);
			},
			//Rotation
			"getLocalRotation", [](const Transform& transform)
			{
				return transform.GetLocalRotation();
			},
			"setLocalRotation", [](Transform& transform, const Quaternion& rotation)
			{
				return transform.SetLocalRotation(rotation);
			},
			"getWorldRotation", [](const Transform& transform)
			{
				return transform.GetWorldRotation();
			},
			"setWorldRotation", [](Transform& transform, const Quaternion& rotation)
			{
				return transform.SetWorldRotation(rotation);
			}
		);
	}

	void LuaComponentsBinder::CreateCameraComponentLuaBind(sol::state& lua)
	{
		lua.new_usertype<CameraComponent>(
			"CameraComponent",
			"type_id", &entt::type_hash<CameraComponent>::value,
			sol::call_constructor,
			sol::factories(
				[]()
				{
					return CameraComponent{};
				}),
			"isMain", &CameraComponent::IsMain,
			"fixAspectRatio", &CameraComponent::FixedAspectRatio
		);
	}

	void LuaComponentsBinder::CreateLuaEntityBind(sol::state& lua, Scene* scene)
	{
		using namespace Scripting;
		using namespace entt::literals;
		lua.new_usertype<Entity>(
			"Entity",
			sol::call_constructor,
			sol::factories(
				[&]()
				{
					return Entity{ scene->CreateEntity() };
				},
				[&](String name)
				{
					return Entity{ scene->CreateEntity(name) };
				}
			),
			"add_component", [](Entity& entity, const sol::table& scriptComponent, sol::this_state state) -> sol::object
			{
				if (!scriptComponent.valid())
				{
					return sol::lua_nil_t{};
				}

				const auto component = InvokeMetaFunction(
					GetIdType(scriptComponent),
					"add_component"_hs,
					entity,
					scriptComponent,
					state);

				return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
			},
			"has_component", [](Entity& entity, const sol::table& scriptComponent)
			{
				const auto result = InvokeMetaFunction(
					GetIdType(scriptComponent),
					"has_component"_hs,
					entity);

				return result ? result.cast<bool>() : false;
			},
			"get_component", [](Entity& entity, const sol::table& scriptComponent, sol::this_state state)
			{
				const auto component = InvokeMetaFunction(
					GetIdType(scriptComponent),
					"get_component"_hs,
					entity,
					state);

				return component ? component.cast<sol::reference>() : sol::lua_nil_t{};
			},
			"remove_component", [](Entity& entity, const sol::table& scriptComponent)
			{
				InvokeMetaFunction(
					GetIdType(scriptComponent),
					"remove_component"_hs,
					entity);
			},
			"name", [](const Entity& entity)
			{
				return entity.GetName().c_str();
			},
			"uuid", [](const Entity& entity)-> uint32_t
			{
				return entity;
			}
		);
	}
}
