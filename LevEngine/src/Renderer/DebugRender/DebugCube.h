#pragma once
#include "DebugShape.h"

namespace LevEngine
{
class DebugCube final : public DebugShape
{
public:
    explicit DebugCube(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override;

private:
    Matrix m_Model{};
};
}
