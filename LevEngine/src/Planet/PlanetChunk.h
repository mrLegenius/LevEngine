#pragma once

#include "Kernel/Core.h"
#include "DataTypes/Array.h"
#include "Math/Math.h"
#include "PlanetChunkBuilder.h"
#include "PlanetGeometry.h"

namespace LevEngine
{
	class Mesh;
	class PlanetSampler;

	// A build in flight. Lives in a Ref rather than in the chunk because a chunk can be merged away
	// while its geometry is still being generated, and the job holds one of these instead of a
	// pointer to the chunk -- so the worker never touches freed memory and the result is simply
	// dropped when nobody is left to want it.
	struct PlanetChunkBuild
	{
		std::atomic<bool> IsDone{false};
		PlanetChunkGeometry Geometry;
	};

	// One node of the quadtree on one cube face.
	//
	// A chunk is a square of face coordinates and a mesh of fixed resolution covering it, so the four
	// children of a chunk carry four times the vertices over the same ground. That is the whole of the
	// level of detail scheme: there is no simplification step and no vertex removal, only a tree that
	// is deeper where the camera is close.
	//
	// The tree is touched from the main thread only. Generation happens on worker threads, but a
	// worker only ever fills a PlanetChunkBuild -- it does not know what a chunk is.
	class LEV_API PlanetChunk
	{
	public:
		// The radius and the shape's height range are needed up front, not after generation: the
		// bounding sphere is what decides whether the chunk is worth generating at all, so it has to
		// be known before it exists. Widening it by the tallest thing the shape can produce is
		// conservative, and once the mesh is there it is replaced by the real bounds.
		PlanetChunk(PlanetFace face, Vector2 minCoordinates, Vector2 maxCoordinates, uint32_t depth,
		            float planetRadius, float elevationRange);

		//<--- Where and how big ---<<

		[[nodiscard]] PlanetFace GetFace() const { return m_Face; }
		[[nodiscard]] uint32_t GetDepth() const { return m_Depth; }
		[[nodiscard]] Vector2 GetMinCoordinates() const { return m_MinCoordinates; }
		[[nodiscard]] Vector2 GetMaxCoordinates() const { return m_MaxCoordinates; }
		[[nodiscard]] Vector3 GetCenterDirection() const { return m_CenterDirection; }

		//<--- Width of the chunk on the surface, in world units. The LOD metric is built on it ---<<
		[[nodiscard]] float GetSize() const { return m_Size; }

		// Centre of the chunk's bounding sphere in planet space, and its radius. Both are known
		// before the chunk is generated -- from the sphere and the shape's height range -- because
		// the decision to generate it is made using them.
		[[nodiscard]] Vector3 GetBoundsCenter() const { return m_BoundsCenter; }
		[[nodiscard]] float GetBoundsRadius() const { return m_BoundsRadius; }

		//<--- The mesh, and the point in planet space its vertices are measured from ---<<
		[[nodiscard]] const Ref<Mesh>& GetMesh() const { return m_Mesh; }
		[[nodiscard]] Vector3 GetOrigin() const { return m_Origin; }
		[[nodiscard]] bool IsReady() const { return m_Mesh != nullptr; }
		[[nodiscard]] bool IsBuilding() const { return m_Build != nullptr; }

		//<--- Kept only when collision is on, and only on leaves. See PlanetSurface ---<<
		[[nodiscard]] const PlanetChunkGeometry* GetGeometry() const { return m_Geometry.get(); }

		[[nodiscard]] bool HasLand() const { return m_MaxElevation > 0.0f; }
		[[nodiscard]] bool HasWater() const { return m_MinElevation < 0.0f; }

		//<--- Tree ---<<

		[[nodiscard]] bool IsSplit() const { return m_Children[0] != nullptr; }
		[[nodiscard]] const Array<Scope<PlanetChunk>, 4>& GetChildren() const { return m_Children; }
		[[nodiscard]] Array<Scope<PlanetChunk>, 4>& GetChildren() { return m_Children; }

		void Split(float planetRadius, float elevationRange);
		void Merge();

		//<--- All four children have a mesh, which is when it is safe to stop drawing this one ---<<
		[[nodiscard]] bool AreChildrenReady() const;

		//<--- Generation ---<<

		// Hands a build to the job system. The sampler is taken by Ref and the result written into a
		// Ref, so neither depends on this chunk still existing when the worker gets to it.
		void RequestBuild(const Ref<PlanetSampler>& sampler, uint32_t resolution, float skirtDepth,
		                  const Ref<std::atomic<uint32_t>>& buildCounter);

		// Main thread. Turns a finished build into a Mesh, which is the part that needs the render
		// device. Returns true on the frame the mesh appears.
		bool TryFinishBuild(bool keepGeometry);

		//<--- Frees the mesh and any geometry, so the chunk is regenerated when next wanted ---<<
		void Invalidate();

	private:
		PlanetFace m_Face;
		uint32_t m_Depth;
		Vector2 m_MinCoordinates;
		Vector2 m_MaxCoordinates;

		Vector3 m_CenterDirection;
		Vector3 m_BoundsCenter;
		float m_BoundsRadius = 0.0f;
		float m_Size = 0.0f;

		Vector3 m_Origin = Vector3::Zero;
		Ref<Mesh> m_Mesh;
		Scope<PlanetChunkGeometry> m_Geometry;

		float m_MinElevation = 0.0f;
		float m_MaxElevation = 0.0f;

		Ref<PlanetChunkBuild> m_Build;
		Ref<std::atomic<uint32_t>> m_BuildCounter;

		Array<Scope<PlanetChunk>, 4> m_Children{};
	};
}
