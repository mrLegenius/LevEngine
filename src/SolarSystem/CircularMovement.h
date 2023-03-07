#pragma once
#include <memory>
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
		float x = sin(angle + initialOffset) * radius;
		float z = cos(angle + initialOffset) * radius;

		angle += speed * deltaTime;
		
		if (centerPoint)
		{
			x += centerPoint->GetPosition().x;
			z += centerPoint->GetPosition().z;
		}

		auto position = transform->GetPosition();
		position.x = x;
		position.z = z;
		transform->SetPosition(position);
	}
};
