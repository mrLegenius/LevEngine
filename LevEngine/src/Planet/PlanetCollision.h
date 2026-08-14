#pragma once

#include "Kernel/Core.h"
#include "DataTypes/UnorderedMap.h"
#include "Math/Math.h"
#include "Physics/Components/FilterLayer.h"

namespace LevEngine
{
	class PlanetChunk;
	class PlanetSurface;
	struct Transform;

	// Keeps PhysX colliders for the ground near whatever is standing on the planet.
	//
	// A planet cannot have collision everywhere. A cooked triangle mesh is a BVH over its triangles,
	// and there are hundreds of chunks visible at any moment -- cooking all of them would cost more
	// than the rest of the frame put together, for ground nothing is within a kilometre of. So the
	// colliders follow the things that could fall onto them, and the rest of the planet is geometry
	// with nothing behind it.
	//
	// Colliders are keyed by which square of which cube face they cover rather than by the chunk that
	// produced them, because chunks are created and destroyed constantly as the camera moves and a
	// freed chunk's address can come back as a different chunk. A face and a pair of coordinates at a
	// depth always mean the same piece of ground.
	class LEV_API PlanetCollision
	{
	public:
		PlanetCollision() = default;
		~PlanetCollision();

		PlanetCollision(const PlanetCollision&) = delete;
		PlanetCollision& operator=(const PlanetCollision&) = delete;

		// focusPoints are in world space -- the things worth having ground under. radius is how far
		// around each of them to keep colliders, in world units.
		void Update(const PlanetSurface& surface, const Transform& transform,
		            const Vector<Vector3>& focusPoints, float radius, FilterLayer layer = FilterLayer::Layer0);

		void Clear();

		[[nodiscard]] uint32_t GetColliderCount() const { return static_cast<uint32_t>(m_Colliders.size()); }

		// Chunks cooked per update. Cooking a chunk is a few milliseconds, and a player walking into
		// unexplored ground wants a handful of chunks at once -- doing them all in the frame they
		// become relevant is a visible stall, and doing one a frame is a fall through the floor.
		static constexpr uint32_t k_MaxCooksPerUpdate = 3;

	private:
		//<--- Which square of which face, at which depth. Unique for the life of the planet ---<<
		[[nodiscard]] static uint64_t GetChunkKey(const PlanetChunk& chunk);

		void Remove(uint64_t key);

		UnorderedMap<uint64_t, physx::PxRigidStatic*> m_Colliders;
	};
}
