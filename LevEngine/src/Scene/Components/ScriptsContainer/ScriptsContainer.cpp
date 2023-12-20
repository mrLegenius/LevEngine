#include "levpch.h"
#include "ScriptsContainer.h"
#include "Scripting/MetaUtilities.h"

namespace LevEngine
{
    sol::object ScriptsContainer::Get(sol::stack_object key, sol::this_state L)
    {
        // we use stack_object for the arguments because we
        // know the values from Lua will remain on Lua's stack,
        // so long we we don't mess with it
        if (auto maybeStringKey = key.as<sol::optional<std::string>>()) {
            if (const String keyString{maybeStringKey.value().c_str()};
                m_ScriptComponents.find(keyString) != m_ScriptComponents.end())
            {
                return sol::object(
                     L,
                     sol::in_place,
                     m_ScriptComponents[keyString]);
            }
        }

        // No valid key: push nil
        // Note that the example above is a bit unnecessary:
        // binding the variables x and y to the usertype
        // would work just as well and we would not
        // need to look it up here,
        // but this is to show it works regardless
        return sol::object(L, sol::in_place, sol::lua_nil);
    }

    void ScriptsContainer::Set(sol::stack_object key, sol::stack_object value, sol::this_state)
    {
        // we use stack_object for the arguments because we
        // know the values from Lua will remain on Lua's stack,
        // so long we we don't mess with it
        if (auto maybeStringKey = key.as<sol::optional<std::string>>()) {
            const String keyString{maybeStringKey.value().c_str()};
            m_ScriptComponents.emplace(eastl::make_pair(keyString, value.as<sol::table>()));
        }
    }
}
