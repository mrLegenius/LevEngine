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
		const float x = sin(angle + initialOffset) * radius;
		const float y = cos(angle + initialOffset) * radius;

		angle += speed * deltaTime;

		transform->position.x = centerPoint->position.x + x;
		transform->position.y = centerPoint->position.y + y;
	}
};
