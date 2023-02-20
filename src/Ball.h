#pragma once
#include "GameObject.h"
#include "Components/Transform.h"
#include "Components/Movement.h"
#include "Components/QuadRenderer.h"


class Ball : public GameObject
{
public:
	explicit Ball(std::shared_ptr<D3D11Shader> shader)
		: GameObject(std::make_shared<QuadRenderer>(shader)),
		m_Movement(std::make_shared<Movement>(m_Transform))
	{
		m_Transform->scale = DirectX::SimpleMath::Vector3::One * 0.01f;
	}

	void Update(const float deltaTime) const
	{
		m_Movement->Update(deltaTime);
	}

	void SetVelocity(DirectX::SimpleMath::Vector3 value) const { m_Movement->velocity = value; }
	void AddSpeed(const float value) const
	{
		m_Movement->velocity.x += std::signbit(m_Movement->velocity.x) ? -value : value;
		m_Movement->velocity.y += std::signbit(m_Movement->velocity.y) ? -value : value;
	}

	void ChangeXDirection() const { m_Movement->velocity.x *= -1; }
	void ChangeYDirection() const { m_Movement->velocity.y *= -1; }

	DirectX::SimpleMath::Vector3 GetVelocity() const { return m_Movement->velocity; }

private:
	std::shared_ptr<Movement> m_Movement;
};
