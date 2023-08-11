#include "Constraint.h"
#include <SimpleMath.h>
#include "../GameObject.h"

void PositionConstraint::Update(const float deltaTime)
{
	const Vector3 relativePos = objectA->GetTransform()->GetLocalPosition() - objectB->GetTransform()->GetLocalPosition();
	const float currentDistance = relativePos.Length();
	const float offset = distance - currentDistance;

	if (abs(offset) <= 0.0f) return;

	Vector3 offsetDir = relativePos;
	offsetDir.Normalize();

	const auto rigidbodyA = objectA->GetRigidbody();
	const auto rigidbodyB = objectB->GetRigidbody();

	const Vector3 relativeVelocity = rigidbodyA->GetLinearVelocity() - rigidbodyB->GetLinearVelocity();

	const float constraintMass = rigidbodyA->GetInverseMass() + rigidbodyB->GetInverseMass();

	if (constraintMass <= 0.0f) return;

	// how much of their relative force is affecting the constraint
	const float velocityDot = relativeVelocity.Dot(offsetDir);

	constexpr float biasFactor = 0.01f;
	const float bias = -(biasFactor / deltaTime) * offset;
	const float lambda = -(velocityDot + bias) / constraintMass;

	const Vector3 aImpulse = offsetDir * lambda;
	const Vector3 bImpulse = -offsetDir * lambda;

	rigidbodyA->AddImpulse(aImpulse); // multiplied by mass here
	rigidbodyB->AddImpulse(bImpulse); // multiplied by mass here
}
