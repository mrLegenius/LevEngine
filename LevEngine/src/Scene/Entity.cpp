#include "levpch.h"
#include "Entity.h"
#include "Scene.h"
#include "Scripting/MetaUtilities.h"

namespace LevEngine
{
template <typename T>
void Entity::AddScript()
{
    LEV_CORE_ASSERT(m_Handle.valid(), "Entity is not valid!");
    LEV_CORE_ASSERT(!HasComponent<T>(), "Entity already has this component!");
}

void Entity::CreateLuaEntityBind(sol::state& lua, Scene* scene)
{
    using namespace Scripting;
    using namespace entt::literals;
    lua.new_usertype<Entity>(
        "Entity",
        sol::call_constructor,
        sol::factories(
            [&]()
            {
                return Entity{ scene->CreateEntity()};
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
