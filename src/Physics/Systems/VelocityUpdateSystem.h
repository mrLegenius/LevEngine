#pragma once
#include "Components/Transform.h"
#include "Physics/Components/Rigidbody.h"

inline void VelocityUpdateSystem(const float deltaTime, entt::registry& registry)
{
	LEV_PROFILE_FUNCTION();

	auto group = registry.group<>(entt::get<Transform, Rigidbody>);
	for (auto entity : group)
	{
		auto [transform, rigidbody] = group.get<Transform, Rigidbody>(entity);

		if (rigidbody.bodyType != BodyType::Dynamic || !rigidbody.enabled) continue;

		const auto inverseMass = rigidbody.GetInverseMass();
		Vector3 acceleration = rigidbody.force * inverseMass;

		if (inverseMass > 0)
			acceleration += gravity * rigidbody.gravityScale;

		rigidbody.velocity += acceleration * deltaTime;

		rigidbody.UpdateInertiaTensor(transform);

		const Vector3 angularAcceleration = Vector3::Transform(rigidbody.torque, rigidbody.GetInertiaTensor());
		//const Vector3 angularAcceleration = torque;

		rigidbody.angularVelocity += angularAcceleration * deltaTime;
	}
}
