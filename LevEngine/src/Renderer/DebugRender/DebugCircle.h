#pragma once
#include "DebugShape.h"
#include "Math/Matrix.h"
#include "Math/Quaternion.h"


namespace LevEngine
{
class DebugCircle final : public DebugShape
{
public:
    explicit DebugCircle(const Matrix& model, const Color color)
    : DebugShape(color), m_Model(model) { }
    
    void Draw() override;

private:
    Matrix m_Model{};
    Quaternion m_Rotation{};
};
}
