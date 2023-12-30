#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
    class ScriptsPanel final : public Panel
    {
        
    protected:
        String GetName() override;
        void DrawContent() override;
    };
}

