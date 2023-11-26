#pragma once
#include "DebugShape.h"
#include "Renderer/3D/Primitives.h"

namespace LevEngine
{
    class Mesh;

    class DebugLine final : public DebugShape
{
public:
    explicit DebugLine(const Vector3 start, const Vector3 end, const Color color)
        : DebugShape(color), m_Mesh(Primitives::CreateLine(start, end)) { }
    
    void Draw() override;

private:
    Ref<Mesh> m_Mesh;
};
}
