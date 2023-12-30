#include "levpch.h"
#include "ScriptingManager.h"
#include "ScriptingSystems.h"
#include "ScriptingSystemComponents.h"

#include "MetaUtilities.h"
#include "MathLuaBindings.h"
#include "LuaComponentsBinder.h"
#include "Assets/ScriptAsset.h"
#include "Physics/Components/Rigidbody.h"

#include "Scene/Scene.h"
#include "Scene/Serializers/SerializerUtils.h"

#include "Scene/Components/Transform/Transform.h"
#include "Scene/Components/Camera/Camera.h"
#include "Scene/Components/ScriptsContainer/ScriptsContainer.h"

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
        LuaComponentsBinder::CreateSceneManagerBind(*m_Lua);
        LuaComponentsBinder::CreateAudioBind(*m_Lua);
        LuaComponentsBinder::CreatePrintBind(*m_Lua);

        LuaComponentsBinder::CreateSceneBind(*m_Lua);
        LuaComponentsBinder::CreatePrefabBind(*m_Lua);
        
        RegisterComponent(ScriptsContainer);
        RegisterComponent(Transform);
        RegisterComponent(CameraComponent);
        RegisterComponent(Rigidbody)
    }

    ScriptingManager::ScriptingManager()
    {
        m_Lua = CreateRef<sol::state>();
    }

    bool ScriptingManager::LoadScripts()
    {
        m_Systems.clear();
        m_Components.clear();

        bool isSuccessful = true;
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
                    Log::Trace("Loading lua script: {0}", scriptName);
                    
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
                    isSuccessful = false;
                }
            }
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load lua scripts. Error: {}", e.what());
            return false;
        }
        
        return isSuccessful;
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

        auto forEach = [&](entt::runtime_view& view, const sol::function& callback)
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
        };

        auto exclude = [&](entt::runtime_view& view, const sol::variadic_args& va)
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
        };
        if (const sol::optional<sol::table> runtimeViewTable = (*m_Lua)["RuntimeView"];
            runtimeViewTable != sol::nullopt)
        {
            auto table = runtimeViewTable.value();
            table.set_function("for_each", forEach);
            table.set_function("exclude", exclude);
        }
        else
        {
            m_Lua->new_usertype<entt::runtime_view>(
            "RuntimeView",
            sol::no_constructor,
            "for_each",
            forEach,
            "exclude",
            exclude,
            "size_hint", &entt::runtime_view::size_hint
        );
        }
        
#pragma warning(push)
#pragma warning(disable : 4996)
        
        auto getEntity = [&](const sol::variadic_args& va) {
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
            };

        auto clear = [&](){ registry.clear(); };
        
        if (const sol::optional<sol::table> registryTable = (*m_Lua)["Registry"];
            registryTable != sol::nullopt)
        {
            auto table = registryTable.value();
            table.set_function("get_entities", getEntity);
            table.set_function("clear", clear);
        }
        else
        {
            m_Lua->new_usertype<entt::registry>(
                "Registry",
                sol::no_constructor,
                "get_entities", getEntity,
                "clear", clear
            );
        }
#pragma warning(pop)
    }

    void ScriptingManager::Shutdown()
    {
        m_Systems.clear();
        m_Components.clear();
    }

    Ref<sol::state> ScriptingManager::GetLuaState()
    {
        return m_Lua;
    }

    Ref<ScriptAsset> ScriptingManager::GetComponentScriptAssetByName(const String& name) const
    {
        for (const auto [scriptAsset, table]: m_Components) {
            if (scriptAsset->GetName() == name) {
                return scriptAsset;
            }
        }

        return nullptr;
    }
    void ScriptingManager::InitScriptsContainers(entt::registry& registry) const
    {
        auto view = registry.view<ScriptsContainer>();

        for (const auto entity : view) 
        {
            auto& scriptContainer = view.get<ScriptsContainer>(entity);

            for (const auto& scriptAsset : scriptContainer.m_ScriptsAssets) 
            {
                sol::optional<sol::protected_function> componentConstructor = m_Components.at(scriptAsset)["new"];
                if (componentConstructor != sol::nullopt)
                {
                    auto result = componentConstructor.value()();
                    if (!result.valid()) 
                    {
                        sol::error err = result;
                        std::string what = err.what();
                        Log::Error("Error while constructing lua component {0}: {1}", scriptAsset->GetName(), what);
                        continue;
                    }

                    sol::table luaComponentInstance = result;
                    scriptContainer.m_ScriptComponents.emplace(
                        eastl::make_pair(scriptAsset->GetName(), luaComponentInstance));
                }
                else
                {
                    Log::CoreError("Error while constructing lua component {0}: no new(...) function", scriptAsset->GetName());
                }
            }
        }
    }
}
