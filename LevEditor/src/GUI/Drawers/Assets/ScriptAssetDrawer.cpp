#include "pch.h"

#include "AssetDrawer.h"
#include "Assets/ScriptAsset.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class ScriptAssetDrawer final : public AssetDrawer<ScriptAsset, ScriptAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "ScriptAsset"; }

        void DrawContent(Ref<ScriptAsset> assetRef) override
        {
            const Array<String, 3> typeValues {"Undefined", "System", "Component"};

            ScriptAsset::Type type = assetRef->GetType();
            if (EditorGUI::DrawComboBox("ScriptType", typeValues, type))
            {
                assetRef->SetType(type);
            }
            
        }
    };
}
