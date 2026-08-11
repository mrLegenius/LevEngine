#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class ScriptsPanel final : public Panel
    {
    public:
        ScriptsPanel() { m_DefaultWindowSize = Vector2{ 230, 660 }; }

    protected:
        String GetName() override;
        void DrawContent() override;
    };
}

