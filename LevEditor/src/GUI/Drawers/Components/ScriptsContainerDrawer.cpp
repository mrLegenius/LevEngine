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

            EditorGUI::DrawSelectableAssetList("Script Components", component.m_ScriptsAssets, m_SelectedAsset);

        }
    private:
        Weak<ScriptAsset> m_SelectedAsset;
    };
}
