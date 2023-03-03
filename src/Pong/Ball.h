#pragma once
#include "GameObject.h"
#include "../Components/Transform.h"
#include "../Components/Movement.h"
#include "../Components/ShapeRenderer.h"

static DirectX::SimpleMath::Vector3 startVelocity = DirectX::SimpleMath::Vector3{0.5f, 0.5f, 0.0f};

class Ball final : public GameObject
{
public:
	explicit Ball(std::shared_ptr<D3D11Shader> shader)
		: GameObject(std::make_shared<ShapeRenderer>(shader, 20)),
		m_Movement(std::make_shared<Movement>(m_Transform))
	{
		m_Transform->scale = DirectX::SimpleMath::Vector3::One * 0.05f;
	}

	void Update(const float deltaTime)
	{
		GameObject::Update(deltaTime);

		m_Movement->Update(deltaTime);
	}

	void Reset() const
	{
		auto& velocity = m_Movement->velocity;
		velocity = startVelocity;

		if (rand() % 2) velocity.x *= -1;
		if (rand() % 2) velocity.y *= -1;

		m_Transform->position = DirectX::SimpleMath::Vector3::Zero;
	}

	void AddVelocity(const DirectX::SimpleMath::Vector3 velocity)
	{
		m_Movement->velocity += velocity;
	}

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
