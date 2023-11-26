#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"

namespace LevEngine
{
class DebugWireSphere final : public DebugShape
{
public:
    explicit DebugWireSphere(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override;

private:
    Matrix m_Model{};
};
}
