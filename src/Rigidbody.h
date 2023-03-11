#pragma once

#include <iostream>
#include <memory>
#include <SimpleMath.h>

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
	Rigidbody(const std::shared_ptr<Transform>& transform) : m_Transform(transform)
	{
		
	}
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

	void AddImpulse(const Vector3 value)
	{
		velocity += value * GetInverseMass();
	}

	void AddAngularImpulse(const Vector3 value)
	{
		angularVelocity += Vector3::Transform(value, inverseInteriaTensor);
	}

	void AddForce(const Vector3 value)
	{
		force += value;
	}

	void AddTorque(const Vector3& value)
	{
		torque += value;
	}

	void AddForceAtPosition(const Vector3& addedForce, const Vector3& position)
	{
		const Vector3 localPos = position - m_Transform->GetWorldPosition();
		force += addedForce;
		torque += localPos.Cross(addedForce);
	}

	void UpdateVelocity(const float deltaTime)
	{
		if (bodyType != BodyType::Dynamic || !enabled) return;

		const auto inverseMass = GetInverseMass();
		Vector3 acceleration = force * inverseMass;

		if (inverseMass > 0)
			acceleration += gravity * gravityScale;

		velocity += acceleration * deltaTime;

		//UpdateInertiaTensor();

		//const Vector3 angularAcceleration = Vector3::Transform(torque, GetInertiaTensor());
		const Vector3 angularAcceleration = torque;

		angularVelocity += angularAcceleration * deltaTime;
	}

	void UpdatePosition(const float deltaTime)
	{
		if (bodyType != BodyType::Dynamic || !enabled) return;

		//Linear movement
		Vector3 position = m_Transform->GetLocalPosition();
		position += velocity * deltaTime;
		m_Transform->SetLocalPosition(position);

		// Linear Damping
		const float dampingFactor = 1.0f - damping;
		const float frameDamping = powf(dampingFactor, deltaTime);
		velocity *= frameDamping;

		//Angular movement
		Quaternion orientation = m_Transform->GetWorldOrientation();

		orientation += orientation * Quaternion(angularVelocity * deltaTime * 0.5f, 0.0f);
		orientation.Normalize();

		m_Transform->SetWorldRotation(orientation);

		// Angular Damping
		const float angularDampingFactor = 1.0f - angularDamping;
		const float angularFrameDamping = powf(angularDampingFactor, deltaTime);
		angularVelocity *= angularFrameDamping;
	}

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

	Vector3& GetAngularVelocity() { return angularVelocity; }
	Vector3& GetLinearVelocity() { return velocity; }
	//Matrix GetInertiaTensor() const { return inverseInteriaTensor; }

	/*void InitCubeInertia()
	{
		const Vector3 dimensions = m_Transform->GetScale();
		const Vector3 dimsSqr = dimensions * dimensions;
		const auto inverseMass = GetInverseMass();
		inverseInertia.x = (12.0f * inverseMass) / (dimsSqr.y + dimsSqr.z);
		inverseInertia.y = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.z);
		inverseInertia.z = (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.y);
	}

	void InitSphereInertia()
	{
		const auto maxElement = max(max(m_Transform->GetScale().x, m_Transform->GetScale().y), m_Transform->GetScale().z);
		const float radius = maxElement;
		const float i = 2.5f * GetInverseMass() / (radius * radius);

		inverseInertia = Vector3(i, i, i);
	}*/
private:
	std::shared_ptr<Transform> m_Transform;

	/*void UpdateInertiaTensor()
	{
		Quaternion q = m_Transform->GetOrientation();

		const Matrix orientation = Matrix::CreateFromQuaternion(q);

		q.Conjugate();
		const Matrix invOrientation = Matrix::CreateFromQuaternion(q);

		inverseInteriaTensor = orientation * Matrix::CreateScale(inverseInertia) * invOrientation;
	}*/
};


