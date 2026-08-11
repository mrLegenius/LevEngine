#pragma once

#include "Kernel/Core.h"
#include "DebugShape.h"

namespace LevEngine
{
class LEV_API DebugWireSphere final : public DebugShape
{
public:
    explicit DebugWireSphere(const Matrix& model, const Color color) : DebugShape(color), m_Model(model) { }
    void Draw() override;

private:
    Matrix m_Model{};
};
}
