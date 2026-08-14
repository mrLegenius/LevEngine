#include "levpch.h"
#include "PlanetCollision.h"

#include "PlanetChunk.h"
#include "PlanetSurface.h"
#include "Kernel/Application.h"
#include "Physics/Physics.h"
#include "DataTypes/Set.h"
#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
	PlanetCollision::~PlanetCollision()
	{
		Clear();
	}

	uint64_t PlanetCollision::GetChunkKey(const PlanetChunk& chunk)
	{
		// A chunk at depth d covers one cell of a 2^d by 2^d grid on its face, so its coordinates in
		// that grid name it exactly. Rounding is safe: the coordinates are produced by repeated halving
		// from 0 and 1, which is exact in binary.
		const auto cells = static_cast<float>(1u << chunk.GetDepth());

		const auto x = static_cast<uint64_t>(chunk.GetMinCoordinates().x * cells + 0.5f);
		const auto y = static_cast<uint64_t>(chunk.GetMinCoordinates().y * cells + 0.5f);

		return static_cast<uint64_t>(chunk.GetFace())
			| static_cast<uint64_t>(chunk.GetDepth()) << 3
			| x << 8
			| y << 32;
	}

	void PlanetCollision::Remove(const uint64_t key)
	{
		const auto it = m_Colliders.find(key);
		if (it == m_Colliders.end()) return;

		App::Get().GetPhysics().RemoveStaticTriangleMesh(it->second);
		m_Colliders.erase(it);
	}

	void PlanetCollision::Clear()
	{
		for (auto& [key, actor] : m_Colliders)
			App::Get().GetPhysics().RemoveStaticTriangleMesh(actor);

		m_Colliders.clear();
	}

	void PlanetCollision::Update(const PlanetSurface& surface, const Transform& transform,
	                            const Vector<Vector3>& focusPoints, const float radius, const FilterLayer layer)
	{
		LEV_PROFILE_FUNCTION();

		if (focusPoints.empty())
		{
			//<--- Nothing on the planet: nothing needs ground under it ---<<
			Clear();
			return;
		}

		const Matrix planetToWorld = transform.GetModel();
		const Quaternion planetRotation = transform.GetWorldRotation();

		const float radiusSquared = radius * radius;

		Set<uint64_t> wanted;
		Vector<const PlanetChunk*> toCook;

		for (const PlanetChunk* chunk : surface.GetVisibleChunks())
		{
			// Only chunks that kept their triangles can be cooked, which is the leaves when the planet
			// has collision switched on. A chunk that is drawn but has been split has children with
			// the geometry instead.
			const PlanetChunkGeometry* geometry = chunk->GetGeometry();
			if (!geometry || geometry->Positions.empty()) continue;

			const Vector3 worldCenter = Vector3::Transform(chunk->GetBoundsCenter(), planetToWorld);

			bool inRange = false;
			for (const Vector3& focus : focusPoints)
			{
				// Measured to the chunk's sphere rather than its middle, so a large chunk somebody is
				// standing on the edge of still counts.
				const float distance = Math::Max(0.0f, (focus - worldCenter).Length() - chunk->GetBoundsRadius());

				if (distance * distance <= radiusSquared)
				{
					inRange = true;
					break;
				}
			}

			if (!inRange) continue;

			const uint64_t key = GetChunkKey(*chunk);
			wanted.insert(key);

			if (m_Colliders.find(key) == m_Colliders.end())
				toCook.emplace_back(chunk);
		}

		//<--- Anything that has moved out of range, or whose chunk has been merged away ---<<
		Vector<uint64_t> stale;
		for (const auto& [key, actor] : m_Colliders)
		{
			if (wanted.find(key) == wanted.end())
				stale.emplace_back(key);
		}

		for (const uint64_t key : stale)
			Remove(key);

		//<--- Then a few new ones, spread over frames ---<<
		const auto cooks = Math::Min<size_t>(toCook.size(), k_MaxCooksPerUpdate);

		for (size_t index = 0; index < cooks; ++index)
		{
			const PlanetChunk* chunk = toCook[index];
			const PlanetChunkGeometry* geometry = chunk->GetGeometry();

			// The vertices go in as they are -- local to the chunk's origin -- and the origin becomes
			// the actor's pose. Cooking them in world space would hand PhysX coordinates in the
			// millions and a BVH built at that magnitude has no precision left for a metre.
			const Vector3 worldOrigin = Vector3::Transform(chunk->GetOrigin(), planetToWorld);

			physx::PxRigidStatic* actor = App::Get().GetPhysics().CreateStaticTriangleMesh(
				geometry->Positions, geometry->Indices, worldOrigin, planetRotation, layer);

			if (!actor) continue;

			m_Colliders[GetChunkKey(*chunk)] = actor;
		}
	}
}
