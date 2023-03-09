#pragma once

enum class ColliderType
{
	Box,
	Sphere,
};

class Collider
{
public:
	const ColliderType& GetType() const { return m_Type; }

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

	Vector3 extents;
};

class SphereCollider : public Collider
{
public:
	SphereCollider() : Collider(ColliderType::Sphere) { }
	SphereCollider(const float radius) : Collider(ColliderType::Sphere), radius(radius) { }

	float radius = 1;
};