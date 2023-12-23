#pragma once

#include <sol/sol.hpp>
#include "DataTypes/Set.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    class ScriptAsset;
    
    REGISTER_PARSE_TYPE(ScriptsContainer);
    
    struct ScriptsContainer
    {
        UnorderedMap<String, sol::table> m_ScriptComponents;
        Set<Ref<ScriptAsset>> m_ScriptsAssets;
        
        sol::object Get(sol::stack_object key, sol::this_state L);

        void Set(sol::stack_object key, sol::stack_object value,
                 sol::this_state);
    };
}

