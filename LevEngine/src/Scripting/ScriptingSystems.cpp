#include "levpch.h"

#include "ScriptingSystems.h"
#include "ScriptingSystemComponents.h"


namespace LevEngine::Scripting {

	void ScriptingInitSystem::Update(float deltaTime, entt::registry& registry)
	{
		auto scriptsView = registry.view<ScriptingInitComponent>();
		for (const auto entity : scriptsView) {
			auto scriptComponent = scriptsView.get<ScriptingInitComponent>(entity);
			auto result = scriptComponent.init();
			if (!result.valid()) {
				sol::error err = result;
				std::string what = err.what();
				Log::Error("Error while running lua system init {0}", what);
			}
		}
	}

	void ScriptingUpdateSystem::Update(float deltaTime, entt::registry& registry)
	{
		auto scriptsView = registry.view<ScriptingUpdateComponent>();
		for (const auto entity : scriptsView) {
			auto scriptComponent = scriptsView.get<ScriptingUpdateComponent>(entity);
			auto result = scriptComponent.update(deltaTime);
			if (!result.valid()) {
				sol::error err = result;
				std::string what = err.what();
				Log::Error("Error while running lua system update {0}", what);
			}
		}
	}

	void ScriptingLateUpdateSystem::Update(float deltaTime, entt::registry& registry)
	{
		auto scriptsView = registry.view<ScriptingLateUpdateComponent>();
		for (const auto entity : scriptsView) {
			auto scriptComponent = scriptsView.get<ScriptingLateUpdateComponent>(entity);
			auto result = scriptComponent.lateUpdate(deltaTime);
			if (!result.valid()) {
				sol::error err = result;
				std::string what = err.what();
				Log::Error("Error while running lua system late update {0}", what);
			}
		}
	}
}
