#pragma once
#include <algorithm>

enum class ColliderType
{
	Box,
	Sphere,
};

class Collider
{
public:
	const ColliderType& GetType() const { return m_Type; }
	virtual float GetRadius() = 0;
	Vector3 offset;
protected:
	Collider(const ColliderType type) : m_Type(type) { }
private:
	ColliderType m_Type;
};

class BoxCollider : public Collider
{
public:
	BoxCollider() : Collider(ColliderType::Box) { }
	BoxCollider(const Vector3& extents) : Collider(ColliderType::Box), extents(extents) { }
	float GetRadius() override { return max(max(extents.x, extents.y), extents.z); }

	Vector3 extents;
};

class SphereCollider : public Collider
{
public:
	SphereCollider() : Collider(ColliderType::Sphere) { }
	SphereCollider(const float radius) : Collider(ColliderType::Sphere), radius(radius) { }
	float GetRadius() override { return radius; }
	float radius = 1;
};