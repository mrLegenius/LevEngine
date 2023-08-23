#pragma once
#include "Math/Math.h"

namespace LevEngine
{
struct BoxCollider final 
{
	Vector3 extents;
	Vector3 offset;

	BoxCollider();
};

struct SphereCollider final
{
	float radius = 1;
	Vector3 offset;

	SphereCollider();
};
}