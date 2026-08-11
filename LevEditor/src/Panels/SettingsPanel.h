#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class SettingsPanel final : public Panel
    {
    public:
        SettingsPanel() { m_DefaultWindowSize = Vector2{ 555, 660 }; }

    protected:
        String GetName() override { return "Settings"; }
        void DrawContent() override;
    };
}