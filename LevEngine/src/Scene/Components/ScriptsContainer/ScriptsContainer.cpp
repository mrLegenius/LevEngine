#include "levpch.h"
#include "ScriptsContainer.h"
#include "Scripting/MetaUtilities.h"
#include "Scripting/ScriptingManager.h"
#include "Assets/ScriptAsset.h"
#include "Kernel/Application.h"
#include "../ComponentSerializer.h"

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
        if (auto maybeStringKey = key.as<sol::optional<std::string>>()) 
        {
            const String keyString{maybeStringKey.value().c_str()};

            const auto& scriptingManager = Application::Get().GetScriptingManager();

            if (auto scriptAsset = scriptingManager.GetComponentScriptAssetByName(keyString))
            {

                if (m_ScriptsAssets.find(scriptAsset) == m_ScriptsAssets.end()) 
                {
                    m_ScriptsAssets.insert(scriptAsset);
                    m_ScriptComponents.emplace(eastl::make_pair(keyString, value.as<sol::table>()));
                }
                else
                {
                    Log::CoreError("Error while adding {0} to ScriptsContainer: asset already exists", keyString);
                }
            }
            else
            {
                Log::CoreError("Error while adding {0} to ScriptsContainer: no sush script asset", keyString);
            }
        }
    }

    class ScriptsContainerSerializer final : public ComponentSerializer<ScriptsContainer, ScriptsContainerSerializer>
    {
    protected:
        const char* GetKey() override { return "ScriptsContainer"; }

        void SerializeData(YAML::Emitter& out, const ScriptsContainer& component) override
        {
            out << YAML::Key << "ScriptsContainer" << YAML::Value;
            out << YAML::BeginMap;
            out << YAML::Key << "ScriptAssets" << YAML::Value << YAML::BeginSeq;

            for (const auto& scriptAsset : component.m_ScriptsAssets) 
            {
                out << YAML::BeginMap;
                out << YAML::Key << "ScriptAssetUUID" << YAML::Value << scriptAsset->GetUUID();
                out << YAML::EndMap;
            }
            
            out << YAML::EndSeq;
            out << YAML::EndMap;
        }

        void DeserializeData(YAML::Node& node, ScriptsContainer& component) override
        {
            if (auto scriptsContainer = node["ScriptsContainer"])
            {
                if (auto scriptAssets = scriptsContainer["ScriptAssets"])
                {
                    for (const auto& scriptAssetUUID : scriptAssets)
                    {
                        auto uuid = scriptAssetUUID["ScriptAssetUUID"].as<uint64_t>();

                        if (auto script = AssetDatabase::GetAsset<ScriptAsset>(uuid))
                        {
                            component.m_ScriptsAssets.insert(script);
                        }
                    }
                }
            }
        }
    };
}
