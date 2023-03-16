#pragma once
#include <SimpleMath.h>
#include "Kernel/Math.h"

using namespace DirectX::SimpleMath;

class BoxCollider final 
{
public:
	Vector3 extents;
	Vector3 offset;
	float GetRadius() { return LevEngine::Math::Max(LevEngine::Math::Max(extents.x, extents.y), extents.z); }
};

class SphereCollider final
{
public:
	float radius = 1;
	Vector3 offset;
	float GetRadius() { return radius; }
};