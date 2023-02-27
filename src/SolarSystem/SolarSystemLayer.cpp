#include "SolarSystemLayer.h"
#include "../Components/Transform.h"

struct CircularMovement
{
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Transform> centerPoint;
	float radius;
	float speed;
	float initialOffset;
	float angle;

	void Update(const float deltaTime)
	{
		const float x = sin(angle + initialOffset) * radius;
		const float y = cos(angle + initialOffset) * radius;

		angle += speed * deltaTime;

		transform->position.x = centerPoint->position.x + x;
		transform->position.y = centerPoint->position.y + y;
	}
};

class Body
{
public:
	explicit Body(const std::shared_ptr<Body>& pivotBody, const std::shared_ptr<CircularMovement>& movement)
		: m_PivotBody(pivotBody),
		m_Movement(movement)
	{
		
	}

	void Update(const float deltaTime) const
	{
		m_Movement->Update(deltaTime);
	}

private:
	std::shared_ptr<Body> m_PivotBody;
	std::shared_ptr<CircularMovement> m_Movement;
};



void SolarSystemLayer::OnAttach()
{

}

void SolarSystemLayer::OnUpdate()
{
}
