#pragma once

#include <SimpleMath.h>
#include "Kernel/Math.h"
#include "Components/Transform.h"

using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Quaternion;
using DirectX::SimpleMath::Matrix;
static Vector3 gravity = Vector3(0, -9.8f, 0);

enum class BodyType
{
	Static,
	Kinematic,
	Dynamic,
};

class Rigidbody
{
public:
	Rigidbody() = default;
	Rigidbody(const Rigidbody&) = default;

	BodyType bodyType = BodyType::Dynamic;

	float gravityScale = 1.0f;
	float mass = 1;
	float elasticity = 0.5f;

	float damping = 0.8f;
	float angularDamping = 0.8f;

	Vector3 force;
	Vector3 velocity;

	Vector3 torque;
	Vector3 angularVelocity;
	Vector3 inverseInertia;
	Matrix inverseInteriaTensor;
	bool enabled = true;

	void AddImpulse(const Vector3 value) { velocity += value * GetInverseMass(); }
	void AddAngularImpulse(const Vector3 value) { angularVelocity += Vector3::Transform(value, inverseInteriaTensor); }
	void AddForce(const Vector3 value) { force += value; }
	void AddTorque(const Vector3& value) { torque += value; }
	void AddForceAtPosition(const Vector3& addedForce, const Vector3& localPosition)
	{
		force += addedForce;
		torque += localPosition.Cross(addedForce);
	}

	[[nodiscard]] Matrix GetInertiaTensor() const { return inverseInteriaTensor; }
	[[nodiscard]] float GetInverseMass() const
	{
		if (bodyType == BodyType::Static) return 0.0f;
		if (mass < 0.0001f) return 1.0f / 0.0001f;
		return 1.0f / mass;
	}

	void ClearForces()
	{
		force = Vector3::Zero;
		torque = Vector3::Zero;
	}

	void InitCubeInertia(const Transform& transform)
	{
		const Vector3 dimensions = transform.GetLocalScale();
		const Vector3 dimsSqr = dimensions * dimensions;
		const auto inverseMass = GetInverseMass();
		inverseInertia.x = (12.0f * inverseMass) / (dimsSqr.y + dimsSqr.z);
		inverseInertia.y = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.z);
		inverseInertia.z = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.y);
	}

	void InitSphereInertia(const Transform& transform)
	{
		auto scale = transform.GetLocalScale();
		const auto maxElement = LevEngine::Math::Max(LevEngine::Math::Max(scale.x, scale.y), scale.z);
		const float radius = maxElement;
		const float i = 2.5f * GetInverseMass() / (radius * radius);

		inverseInertia = Vector3(i, i, i);
	}

	void UpdateInertiaTensor(const Transform& transform)
	{
		Quaternion q = transform.GetLocalOrientation();

		const Matrix orientation = Matrix::CreateFromQuaternion(q);

		q.Conjugate();
		const Matrix invOrientation = Matrix::CreateFromQuaternion(q);

		inverseInteriaTensor = invOrientation * Matrix::CreateScale(inverseInertia) * orientation;
	}
};


