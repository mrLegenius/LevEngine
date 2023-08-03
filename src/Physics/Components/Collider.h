#pragma once

namespace LevEngine
{
class BoxCollider final 
{
public:
	Vector3 extents;
	Vector3 offset;
};

class SphereCollider final
{
public:
	float radius = 1;
	Vector3 offset;
};
}