#pragma once
#include "GameObject.h"
#include "Components/Movement.h"

class Pad : public GameObject
{
public:
	explicit Pad(float xPosition, std::shared_ptr<D3D11Shader> shader)
		: GameObject(std::make_shared<QuadRenderer>(shader)),
		m_Movement(std::make_shared<Movement>(m_Transform))
	{
		m_Transform->scale = DirectX::SimpleMath::Vector3{ 0.01f, 0.1f, 1.0f };
		m_Transform->position.x = xPosition;
	}

	virtual ~Pad() = default;
	void Update(float deltaTime)
	{
		if (ShouldGoDown())
			m_Movement->velocity = DirectX::SimpleMath::Vector3{ 0.0f, -1.0f, 0.0f };
		else if (ShouldGoUp())
			m_Movement->velocity = DirectX::SimpleMath::Vector3{ 0.0f, 1.0f, 0.0f };
		else
			m_Movement->velocity = DirectX::SimpleMath::Vector3{ 0.0f, 0.0f, 0.0f };

		m_Movement->Update(deltaTime);
	}

protected:
	virtual bool ShouldGoDown() = 0;
	virtual bool ShouldGoUp() = 0;

	std::shared_ptr<Movement> m_Movement;
};
