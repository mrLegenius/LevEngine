#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"
#include "Scene/Components/ScriptsContainer/ScriptsContainer.h"
#include "Assets/ScriptAsset.h"
#include "Scripting/ScriptingManager.h"

namespace LevEngine::Editor
{
    class ScripsContainerDrawer final : public ComponentDrawer<ScriptsContainer, ScripsContainerDrawer>
    {
    protected:
        String GetLabel() const override { return "ScriptsContainer"; }

        void DrawContent(ScriptsContainer& component) override
        {
            auto scriptManager = Application::Get().GetScriptingManager();
            Set<Ref<ScriptAsset>> scriptAssets;
            for (const auto& [idType, table] : component.Components)
            {
                if (Ref<ScriptAsset> scriptAsset;
                    scriptManager.GetComponentAsset(idType, scriptAsset))
                {
                    scriptAssets.insert(scriptAsset);
                }
            }
            if (EditorGUI::DrawSelectableAssetList("Script Components", scriptAssets, m_SelectedAsset))
            {
                component.Components.clear();
                for (const auto& asset : scriptAssets)
                {
                    if (asset->GetType() == ScriptAsset::Type::Component)
                    {
                        auto componentId = entt::hashed_string{asset->GetName().c_str()}.value();
                        if(sol::table scriptComponent;
                            scriptManager.CreateLuaScriptComponent(componentId, scriptComponent))
                        {
                            component.Components.emplace(eastl::make_pair(componentId, eastl::move(scriptComponent)));
                        }
                    }
                }
            }
        }
    private:
        Ref<ScriptAsset> m_SelectedAsset;
    };
}
