#pragma once
#include <memory>

#include "Transform.h"

struct Movement
{
	DirectX::SimpleMath::Vector3 velocity;

	explicit Movement(const std::shared_ptr<Transform>& transform) : m_Transform(transform)
	{

	}

	void Update(const float deltaTime) const
	{
		m_Transform->position.x += velocity.x * deltaTime;
		m_Transform->position.y += velocity.y * deltaTime;
		m_Transform->position.z += velocity.z * deltaTime;
	}

	std::shared_ptr<Transform> m_Transform;
};
