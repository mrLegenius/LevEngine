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
		auto position = m_Transform->GetPosition();

		position.x += velocity.x * deltaTime;
		position.y += velocity.y * deltaTime;
		position.z += velocity.z * deltaTime;

		m_Transform->SetPosition(position);
	}

	std::shared_ptr<Transform> m_Transform;
};
