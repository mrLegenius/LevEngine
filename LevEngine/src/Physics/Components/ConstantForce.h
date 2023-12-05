#pragma once
#include "Math/Vector3.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(ConstantForce);
    
    struct ConstantForce
    {
        [[nodiscard]] Vector3 GetForce() const;
        void SetForce(Vector3 value);
        [[nodiscard]] Vector3 GetTorque() const;
        void SetTorque(Vector3 value);

    private:
        Vector3 m_Force = Vector3::Zero;
        Vector3 m_Torque = Vector3::Zero;
    };
}