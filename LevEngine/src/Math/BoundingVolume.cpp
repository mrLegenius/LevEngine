#include "levpch.h"
#include "BoundingVolume.h"

#include "Renderer/RenderSettings.h"
#include "Renderer/DebugRender/DebugRender.h"

namespace LevEngine
{
    bool AABBBoundingVolume::IsOnFrustum(const Frustum& camFrustum, const Transform& transform) const
    {
        const Vector3 worldCenter{ Vector3::Transform(center, transform.GetModel()) };
        Vector3 worldExtents;

        const Vector3 right = transform.GetModel().Right() * extents.x;
        const Vector3 up = transform.GetModel().Up() * extents.y;
        const Vector3 forward = transform.GetModel().Forward() * extents.z;

        worldExtents.x = std::abs(Vector3{ 1.f, 0.f, 0.f }.Dot(right)) +
            std::abs(Vector3{ 1.f, 0.f, 0.f }.Dot(up)) +
            std::abs(Vector3{ 1.f, 0.f, 0.f }.Dot(forward));

        worldExtents.y = std::abs(Vector3{ 0.f, 1.f, 0.f }.Dot(right)) +
            std::abs(Vector3{ 0.f, 1.f, 0.f }.Dot(up)) +
            std::abs(Vector3{ 0.f, 1.f, 0.f }.Dot(forward));

        worldExtents.z = std::abs(Vector3{ 0.f, 0.f, 1.f }.Dot(right)) +
            std::abs(Vector3{ 0.f, 0.f, 1.f }.Dot(up)) +
            std::abs(Vector3{ 0.f, 0.f, 1.f }.Dot(forward));
		
        const AABBBoundingVolume globalAABB(worldCenter, worldExtents.x, worldExtents.y, worldExtents.z);

        if constexpr (RenderDebugSettings::DrawBoundingVolumes)
            DebugRender::DrawWireCube(worldCenter, worldExtents * 2, Color::Yellow);
		
        return globalAABB.IsOnOrForwardPlane(camFrustum.leftFace)
            && globalAABB.IsOnOrForwardPlane(camFrustum.rightFace)
            && globalAABB.IsOnOrForwardPlane(camFrustum.topFace)
            && globalAABB.IsOnOrForwardPlane(camFrustum.bottomFace)
            && globalAABB.IsOnOrForwardPlane(camFrustum.nearFace)
            && globalAABB.IsOnOrForwardPlane(camFrustum.farFace);
    }
}