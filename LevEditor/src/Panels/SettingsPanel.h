#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class SettingsPanel final : public Panel
    {
    protected:
        String GetName() override { return "Settings"; }
        void DrawContent() override;
    };
}