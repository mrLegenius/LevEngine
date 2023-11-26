#include "levpch.h"
#include "ScriptingManager.h"
#include "Scene/Serializers/SerializerUtils.h"
#include "ScriptingSystems.h"
#include "ScriptingComponent.h"
#include "MetaUtilities.h"
#include "Scene/Scene.h"
#include "MathLuaBindings.h"
#include "LuaComponentsBinder.h"

#include "Scene/Components/Camera/Camera.h"

#define RegisterComponent(x)    LuaComponentsBinder::Create##x##LuaBind(*m_Lua);\
                                LuaComponentsBinder::RegisterMetaComponent<x>();\
                                RegisterMetaComponent<x>();

const String ScriptsDatabaseFilename = "ScriptsDatabase.asset";
const String ScriptsDatabaseDir = "scripts";

namespace LevEngine::Scripting
{

    void ScriptingManager::Init()
    {
        m_Lua->open_libraries(sol::lib::base, sol::lib::package);

        MathLuaBindings::CreateLuaBindings(*m_Lua);
        LuaComponentsBinder::CreateInputLuaBind(*m_Lua);
        
        RegisterComponent(Transform);
        RegisterComponent(CameraComponent);

        LoadScripts("");
    }

    ScriptingManager::ScriptingManager()
    {
        m_Lua = CreateRef<sol::state>();
    }

    bool ScriptingManager::LoadScripts(const Path& projectPath)
    {
        try
        {
            YAML::Node data = YAML::LoadFile((projectPath / ScriptsDatabaseFilename.c_str()).string().c_str());

            if (!data["Scripts"]) return true;

            auto scripts = data["Scripts"];

            const Path scriptsDir = (projectPath / ScriptsDatabaseDir.c_str());

            for (const auto& script : scripts)
            {
	            auto scriptName = script.as<String>();
                try
                {
                    auto result = m_Lua->safe_script_file((scriptsDir / (scriptName + ".m_Lua").c_str()).string().c_str());
                    Log::CoreInfo("Loading m_Lua script: {0}", scriptName);

                    sol::table system = (*m_Lua)[scriptName.c_str()];
                    
                    m_Systems.emplace(eastl::make_pair(scriptName, eastl::move(system)));
                }
                catch (const sol::error& err)
                {
                    Log::CoreError("Error loading m_Lua script {0}: {1}", scriptName, err.what());
                    return false;
                }
            }
        }
        catch (std::exception& e)
        {
            Log::CoreWarning("Failed to load m_Lua scripts. Error: {}", e.what());
            return false;
        }

        return true;
    }

    void ScriptingManager::RegisterSystems(Scene* const scene)
    {
        for (const auto& [name, system] : m_Systems)
        {
            auto scriptingEntity = scene->CreateEntity("ScriptingEntity_" + name);
                
            if (sol::optional<sol::protected_function> init = system["init"]; 
                init != sol::nullopt)
            {
                auto& scriptingComponent = scriptingEntity.AddComponent<ScriptingInitComponent>();
                scriptingComponent.init = init.value();
            }

                
            if (sol::optional<sol::protected_function> update = system["update"]; 
                update != sol::nullopt)
            {
                auto& scriptingComponent = scriptingEntity.AddComponent<ScriptingUpdateComponent>();
                scriptingComponent.update = update.value();
            }

                
            if (sol::optional<sol::protected_function> lateUpdate = system["lateUpdate"]; 
                lateUpdate != sol::nullopt)
            {
                auto& scriptingComponent = scriptingEntity.AddComponent<ScriptingLateUpdateComponent>();
                scriptingComponent.lateUpdate = lateUpdate.value();
            }
        }

        //TODO Register Init System
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
