#include "pch.h"
#include "ScriptsPanel.h"

#include "Assets/ScriptAsset.h"
#include "GUI/EditorGUI.h"


namespace LevEngine::Editor
{
    String ScriptsPanel::GetName()
    {
        return "Scripts";
    }

    void ScriptsPanel::DrawContent()
    {
        LEV_PROFILE_FUNCTION();
		
        const auto& activeScene = SceneManager::GetActiveScene();

        if (!activeScene) return;

        auto scripts = AssetDatabase::GetAllAssetsOfClass<ScriptAsset>();

        for (const auto& script : scripts)
        {
            if (script->GetType() != ScriptAsset::Type::System)
            {
                continue;
            }
            
            if (bool isActive = activeScene->IsScriptSystemActive(script); EditorGUI::DrawCheckBox(script->GetName().c_str(), isActive))
            {
                activeScene->SetScriptSystemActive(script, isActive);
            }
        }
    }
}
