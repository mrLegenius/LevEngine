#include "levpch.h"
#include "ScriptingManager.h"
#include "ScriptingSystems.h"
#include "ScriptingComponent.h"

#include "MetaUtilities.h"
#include "MathLuaBindings.h"
#include "LuaComponentsBinder.h"
#include "Assets/ScriptAsset.h"

#include "Scene/Scene.h"
#include "Scene/Serializers/SerializerUtils.h"

#include "Scene/Components/Transform/Transform.h"
#include "Scene/Components/Camera/Camera.h"

#define RegisterComponent(x)    LuaComponentsBinder::Create##x##LuaBind(*m_Lua);\
                                LuaComponentsBinder::RegisterMetaComponent<x>();\
                                RegisterMetaComponent<x>();

namespace LevEngine::Scripting
{

    void ScriptingManager::Init()
    {
        m_Lua->open_libraries(sol::lib::base, sol::lib::package);

        MathLuaBindings::CreateLuaBindings(*m_Lua);
        LuaComponentsBinder::CreateInputLuaBind(*m_Lua);
        
        RegisterComponent(Transform);
        RegisterComponent(CameraComponent);
    }

    ScriptingManager::ScriptingManager()
    {
        m_Lua = CreateRef<sol::state>();
    }

    bool ScriptingManager::LoadScripts()
    {
        try
        {
            auto scripts = AssetDatabase::GetAllAssetsOfClass<ScriptAsset>();

            for (const auto& script : scripts)
            {
                script->Deserialize();
                auto scriptName = script->GetName();
                try
                {
                    auto result = m_Lua->safe_script_file(script->GetPath().string());
                    Log::CoreInfo("Loading lua script: {0}", scriptName);
                    
                    sol::optional<sol::table> luaScriptTable = (*m_Lua)[scriptName.c_str()];
                    if (luaScriptTable == sol::nullopt)
                    {
                        Log::CoreError("Error loading lua script {0}: lua table should be named as lua script file", scriptName);
                    }
                    else
                    {
                        switch (script->GetType()) {
                        case ScriptAsset::Type::System:
                            m_Systems.emplace(make_pair(script, eastl::move(luaScriptTable.value())));
                            break;
                        case ScriptAsset::Type::Component:
                            m_Components.emplace(make_pair(script, eastl::move(luaScriptTable.value())));
                            break;
                        default: ;
                        }
                    }                    
                }
                catch (const sol::error& err)
                {
                    Log::CoreError("Error loading lua script {0}: {1}", scriptName, err.what());
                    return false;
                }
            }
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load lua scripts. Error: {}", e.what());
            return false;
        }

        return true;
    }

    void ScriptingManager::RegisterSystems(Scene* scene)
    {
        for (const auto& [scriptAsset, luaSystemTable] : m_Systems)
        {
            if (!scene->IsScriptSystemActive(scriptAsset))
            {
                continue;
            }
            
            auto name = scriptAsset->GetName();
            auto scriptingEntity = scene->CreateEntity("ScriptingEntity_" + name);
                
            if (sol::optional<sol::protected_function> init = luaSystemTable["init"]; 
                init != sol::nullopt)
            {
                auto& scriptingComponent = scriptingEntity.AddComponent<ScriptingInitComponent>();
                scriptingComponent.init = init.value();
            }

                
            if (sol::optional<sol::protected_function> update = luaSystemTable["update"]; 
                update != sol::nullopt)
            {
                auto& scriptingComponent = scriptingEntity.AddComponent<ScriptingUpdateComponent>();
                scriptingComponent.update = update.value();
            }

                
            if (sol::optional<sol::protected_function> lateUpdate = luaSystemTable["lateUpdate"]; 
                lateUpdate != sol::nullopt)
            {
                auto& scriptingComponent = scriptingEntity.AddComponent<ScriptingLateUpdateComponent>();
                scriptingComponent.lateUpdate = lateUpdate.value();
            }
        }

        scene->RegisterInitSystem<ScriptingInitSystem>();
        scene->RegisterUpdateSystem<ScriptingUpdateSystem>();
        scene->RegisterLateUpdateSystem<ScriptingLateUpdateSystem>();
    }

    void ScriptingManager::CreateRegistryBind(entt::registry& registry) const
    {
        using namespace entt::literals;


        m_Lua->new_usertype<entt::runtime_view>(
            "runtime_view",
            sol::no_constructor,
            "for_each",
            [&](const entt::runtime_view& view, const sol::function& callback)
            {
                if (!callback.valid())
                {
                    return;
                }

                for (auto entityType : view)
                {
                    Entity entity{ {registry, entityType} };
                    callback(entity);
                }
            },
            "exclude",
            [&](entt::runtime_view& view, const sol::variadic_args& va)
            {
                for (const auto& type : va)
                {
                    if (!type.as<sol::table>().valid())
                    {
                        const auto excluded_view = InvokeMetaFunction(
                            GetIdType(type),
                            "exclude_component_from_view"_hs,
                            &registry,
                            view
                        );

                        view = excluded_view ? excluded_view.cast<entt::runtime_view>() : view;
                    }
                }
            }
        );

#pragma warning(push)
#pragma warning(disable : 4996)

        m_Lua->new_usertype<entt::registry>(
            "Registry",
            sol::no_constructor,
            "get_entities", [&](const sol::variadic_args& va) {
                entt::runtime_view view{};
                for (const auto& type : va)
                {
                    if (!type.as<sol::table>().valid())
                        continue;

                    const auto entities = InvokeMetaFunction(
                        GetIdType(type),
                        "add_component_to_view"_hs,
                        &registry,
                        view
                    );

                    view = entities ? entities.cast<entt::runtime_view>() : view;
                }

                return view;
            },
            "clear", [&]() { registry.clear(); }
        );

#pragma warning(pop)
    }

    void ScriptingManager::Shutdown()
    {
        m_Systems.clear();
    }

    Ref<sol::state> ScriptingManager::GetLuaState()
    {
        return m_Lua;
    }
}
