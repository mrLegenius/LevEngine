#pragma once

#include "Rigidbody.h"
#include "Math/Vector3.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
    REGISTER_PARSE_TYPE(Force);

    struct Rigidbody;
    
    struct Force
    {
        enum class Type
        {
            Force,
            Impulse,
            Velocity,
            Acceleration
        };

        [[nodiscard]] Type GetForceType() const;
        void SetForceType(const Type& type);

        [[nodiscard]] bool IsCompletedAction() const;
        void CompleteAction(bool flag);
        
        [[nodiscard]] Vector3 GetLinearForce() const;
        void SetLinearForce(Vector3 value);
        [[nodiscard]] Vector3 GetAngularForce() const;
        void SetAngularForce(Vector3 value);

        void ApplyForce(const Rigidbody& rigidbody);

    private:
        Type m_Type = Type::Force;

        bool m_IsCompletedAction = false;

        Vector3 m_LinearForce = Vector3::Zero;
        Vector3 m_AngularForce = Vector3::Zero;
    };
}
