#include "pch.h"

#include "ComponentDrawer.h"
#include "GUI/EditorGUI.h"
#include "Scene/Components/ScriptsContainer/ScriptsContainer.h"

namespace LevEngine::Editor
{
    class ScripsContainerDrawer final : public ComponentDrawer<ScriptsContainer, ScripsContainerDrawer>
    {
    protected:
        String GetLabel() const override { return "ScriptsContainer"; }

        void DrawContent(ScriptsContainer& component) override
        {
        }
    };
}
