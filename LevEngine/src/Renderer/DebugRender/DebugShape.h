#pragma once

#include "Kernel/Core.h"

namespace LevEngine
{
    class LEV_API DebugShape
    {
    public:
        virtual ~DebugShape() = default;
		
        virtual void Draw() = 0;
        [[nodiscard]] const Color& GetColor() const { return m_Color; }

    protected:
        explicit DebugShape(const Color color) : m_Color(color) { }
        
        Color m_Color;
    };
}
