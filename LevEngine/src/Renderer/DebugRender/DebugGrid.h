#pragma once
#include "DebugShape.h"
#include "Renderer/3D/Primitives.h"

namespace LevEngine
{
    class Mesh;

    class DebugGrid final : public DebugShape
    {
    public:
        explicit DebugGrid(const Matrix& model,
                           Vector3 xAxis, Vector3 yAxis,
                           uint32_t xDivisions, uint32_t yDivisions,
                           const Color color)
            : DebugShape(color), m_Mesh(Primitives::CreateGrid(xAxis, yAxis, xDivisions, yDivisions)), m_Model(model)
        { }

        void Draw() override;

    private:
        Ref<Mesh> m_Mesh;
        Matrix m_Model{};
    };
}
