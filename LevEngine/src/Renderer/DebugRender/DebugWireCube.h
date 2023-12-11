#pragma once
#include "DebugShape.h"

namespace LevEngine
{
class DebugWireCube final : public DebugShape
{
public:
    explicit DebugWireCube(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override;

private:
    Matrix m_Model{};
};
}
