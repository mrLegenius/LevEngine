#pragma once

#include "Kernel/Core.h"
#include "DebugShape.h"

namespace LevEngine
{
class LEV_API DebugCube final : public DebugShape
{
public:
    explicit DebugCube(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override;

private:
    Matrix m_Model{};
};
}
