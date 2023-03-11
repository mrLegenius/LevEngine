#pragma once
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class GameObject;

class Constraint
{
public:

	virtual ~Constraint() = default;

	virtual void Update(float deltaTime) = 0;
};

class PositionConstraint : public Constraint
{
public:
	PositionConstraint(GameObject* objectA, GameObject* objectB, const float distance)
		: objectA(objectA),
		objectB(objectB),
		distance(distance) { }

	void Update(float deltaTime) override;

protected:
	GameObject* objectA;
	GameObject* objectB;
	float distance;
};