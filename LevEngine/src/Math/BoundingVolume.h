#pragma once
#include "Frustum.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
struct BoundingVolume
{
    virtual ~BoundingVolume() = default;
    virtual bool IsOnFrustum(const Frustum& frustum, const Transform& transform) const = 0;
};

struct SphereBoundingVolume final : BoundingVolume
{
    Vector3 center{ 0.f, 0.f, 0.f };
    float radius{ 0.f };

    SphereBoundingVolume(const Vector3& center, const float radius) : center{center}, radius{radius} { }

    [[nodiscard]] bool IsOnOrForwardPlane(const Plane& plane) const
    {
        return plane.GetSignedDistanceToPlane(center) > -radius;
    }

	[[nodiscard]] bool IsOnFrustum(const Frustum& camFrustum) const
    {
    	//Check Firstly the result that have the most chance to failure to avoid to call all functions.
    	return IsOnOrForwardPlane(camFrustum.leftFace)
			&& IsOnOrForwardPlane(camFrustum.rightFace)
			&& IsOnOrForwardPlane(camFrustum.farFace)
			&& IsOnOrForwardPlane(camFrustum.nearFace)
			&& IsOnOrForwardPlane(camFrustum.topFace)
			&& IsOnOrForwardPlane(camFrustum.bottomFace);
    };
	
    [[nodiscard]] bool IsOnFrustum(const Frustum& camFrustum, const Transform& transform) const override
    {
        const Vector3 worldScale = transform.GetWorldScale();
        const Vector3 worldCenter = Vector3::Transform(center, transform.GetModel());
    	
        const float maxScale = Math::MaxElement(worldScale);
    	
        const SphereBoundingVolume globalSphere(worldCenter, radius * maxScale);

        //Check Firstly the result that have the most chance to failure to avoid to call all functions.
        return globalSphere.IsOnOrForwardPlane(camFrustum.leftFace)
            && globalSphere.IsOnOrForwardPlane(camFrustum.rightFace)
            && globalSphere.IsOnOrForwardPlane(camFrustum.farFace)
            && globalSphere.IsOnOrForwardPlane(camFrustum.nearFace)
            && globalSphere.IsOnOrForwardPlane(camFrustum.topFace)
            && globalSphere.IsOnOrForwardPlane(camFrustum.bottomFace);
    };
};

struct AABBBoundingVolume final :  BoundingVolume
{
	Vector3 center{ 0.f, 0.f, 0.f };
	Vector3 extents{ 0.f, 0.f, 0.f };

	AABBBoundingVolume() { }
	AABBBoundingVolume(const Vector3& min, const Vector3& max)
		: center{ (max + min) * 0.5f }, extents{ max.x - center.x, max.y - center.y, max.z - center.z }
	{}

	AABBBoundingVolume(const Vector3& center, const float i, const float j, const float k)
		: center{ center }, extents{ i, j, k }
	{}

	Vector3 GetMin() const
	{
		return {center.x - extents.x, center.y - extents.y, center.z - extents.z};
	}

	Vector3 GetMax() const
	{
		return {center.x + extents.x, center.y + extents.y, center.z + extents.z};
	}
	
	std::array<Vector3, 8> GetVertices() const
	{
		std::array<Vector3, 8> vertices;
		vertices[0] = Vector3{ center.x - extents.x, center.y - extents.y, center.z - extents.z };
		vertices[1] = Vector3{ center.x + extents.x, center.y - extents.y, center.z - extents.z };
		vertices[2] = Vector3{ center.x - extents.x, center.y + extents.y, center.z - extents.z };
		vertices[3] = Vector3{ center.x + extents.x, center.y + extents.y, center.z - extents.z };
		vertices[4] = Vector3{ center.x - extents.x, center.y - extents.y, center.z + extents.z };
		vertices[5] = Vector3{ center.x + extents.x, center.y - extents.y, center.z + extents.z };
		vertices[6] = Vector3{ center.x - extents.x, center.y + extents.y, center.z + extents.z };
		vertices[7] = Vector3{ center.x + extents.x, center.y + extents.y, center.z + extents.z };
		return vertices;
	}

	[[nodiscard]] bool IsOnOrForwardPlane(const Plane& plane) const
	{
		// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
		const float r = extents.x * std::abs(plane.normal.x)
					  + extents.y * std::abs(plane.normal.y)
					  + extents.z * std::abs(plane.normal.z);

		return -r <= plane.GetSignedDistanceToPlane(center);
	}

	[[nodiscard]] bool IsOnFrustum(const Frustum& camFrustum, const Transform& transform) const override;
};
}

