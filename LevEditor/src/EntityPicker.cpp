#include "pch.h"
#include "EntityPicker.h"

#include "EditorMath.h"
#include "Assets/MeshAsset.h"
#include "Renderer/3D/Mesh.h"
#include "Scene/Components/Animation/AnimatorComponent.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"

namespace LevEngine::Editor
{
	namespace
	{
		constexpr float k_NoHit = std::numeric_limits<float>::max();

		//<--- Testing the bounds first keeps the triangle loop out of the way for the meshes the ray misses ---<<
		bool IsInBounds(Mesh& mesh, const Ray& ray, float& distance)
		{
			const auto& bounds = mesh.GetAABBBoundingVolume();
			const DirectX::BoundingBox box{ bounds.center, bounds.extents };

			return ray.Intersects(box, distance);
		}

		float GetTrianglesDistance(const Mesh& mesh, const Ray& ray)
		{
			const auto indicesCount = mesh.GetIndicesCount();
			const auto verticesCount = mesh.GetVerticesCount();

			float closest = k_NoHit;

			for (uint32_t index = 0; index + 2 < indicesCount; index += 3)
			{
				const auto first = mesh.GetIndex(index);
				const auto second = mesh.GetIndex(index + 1);
				const auto third = mesh.GetIndex(index + 2);

				if (first >= verticesCount || second >= verticesCount || third >= verticesCount) continue;

				float distance;
				if (!ray.Intersects(mesh.GetVertex(first), mesh.GetVertex(second), mesh.GetVertex(third), distance))
					continue;

				closest = Math::Min(closest, distance);
			}

			return closest;
		}

		//<--- The ray is in the local space of the mesh, so the distance it returns is in that space as well ---<<
		float GetMeshDistance(const Ref<Mesh>& mesh, const Ray& ray, const bool boundsOnly)
		{
			if (!mesh) return k_NoHit;

			float closest = k_NoHit;

			float boundsDistance;
			//<--- A mesh built out of submeshes only has no geometry and no bounds of its own ---<<
			if (mesh->GetVerticesCount() > 0 && IsInBounds(*mesh, ray, boundsDistance))
			{
				closest = boundsOnly ? boundsDistance : GetTrianglesDistance(*mesh, ray);
			}

			//<--- A mesh renderer draws the whole submesh tree with a single transform ---<<
			for (const auto& subMesh : mesh->GetSubMeshes())
			{
				closest = Math::Min(closest, GetMeshDistance(subMesh, ray, boundsOnly));
			}

			return closest;
		}

		float GetEntityDistance(const Entity entity, const Ray& ray)
		{
			if (!entity.HasComponent<MeshRendererComponent>()) return k_NoHit;

			const auto& meshRenderer = entity.GetComponent<MeshRendererComponent>();
			if (!meshRenderer.enabled || !meshRenderer.mesh) return k_NoHit;

			const auto& mesh = meshRenderer.mesh->GetMesh();
			if (!mesh) return k_NoHit;

			const Matrix inverseModel = GetWorldMatrix(entity.GetComponent<Transform>()).Invert();

			//<--- The ray goes into the local space of the mesh, so the vertices are tested as they are stored ---<<
			const Vector3 origin = Vector3::Transform(ray.position, inverseModel);
			Vector3 direction = Vector3::TransformNormal(ray.direction, inverseModel);

			//<--- An entity scaled down to nothing has no local space to cast the ray in ---<<
			const float directionLength = direction.Length();
			if (directionLength < Math::FloatEpsilon) return k_NoHit;

			direction /= directionLength;

			//<--- The vertices of an animated mesh are kept in the bind pose, so only its bounds are meaningful ---<<
			const bool boundsOnly = entity.HasComponent<AnimatorComponent>();

			const float distance = GetMeshDistance(mesh, Ray{ origin, direction }, boundsOnly);
			if (distance == k_NoHit) return k_NoHit;

			//<--- Scaling the distance back makes the hits on differently scaled entities comparable ---<<
			return distance / directionLength;
		}
	}

	Entity EntityPicker::Pick(const Ref<Scene>& scene, const Ray& ray)
	{
		LEV_PROFILE_FUNCTION();

		if (!scene) return Entity{};

		Entity picked{};
		float closest = k_NoHit;

		scene->ForEachEntityUnordered([&picked, &closest, &ray](const Entity entity)
		{
			const float distance = GetEntityDistance(entity, ray);

			if (distance >= closest) return;

			closest = distance;
			picked = entity;
		});

		return picked;
	}
}
