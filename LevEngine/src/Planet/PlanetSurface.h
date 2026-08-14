#pragma once

#include "Kernel/Core.h"
#include "DataTypes/Array.h"
#include "PlanetChunk.h"
#include "PlanetSampler.h"

namespace LevEngine
{
	// How finely the surface is cut up, and how eagerly.
	struct LEV_API PlanetLodSettings
	{
		// Quads per side of a chunk. This is a straight trade of draw calls against vertices: at 32
		// a chunk is two thousand triangles, and a planet in view is a few hundred chunks. Raising it
		// covers the same ground with fewer, larger chunks, which is cheaper to submit and slower to
		// react to the camera moving, because a chunk is the unit that gets rebuilt.
		uint32_t ChunkResolution = 32;

		// How many times a cube face may be halved. Each level halves the size of a chunk, so the
		// finest detail is the planet's circumference over (4 * 2^MaxDepth * ChunkResolution). Eight
		// levels on a four kilometre planet puts vertices about a third of a unit apart, which is
		// walking scale.
		uint32_t MaxDepth = 8;

		// A chunk splits when the camera is nearer than its own width times this. Two is about one
		// chunk per screen-width at a sixty degree field of view: below it the horizon visibly
		// tessellates as you turn, above it the triangle count climbs as the square.
		float LodBias = 2.0f;

		// Depth of the wall around each chunk, as a fraction of the chunk's width. It has to be at
		// least the largest height difference two neighbouring levels of detail can disagree by,
		// which scales with the chunk -- hence a fraction rather than a distance.
		float SkirtDepthScale = 0.12f;

		// How much coarser the geometry that casts shadows may be than the geometry that is drawn.
		//
		// A shadow map holds far less detail than the view does -- its texels are much larger than the
		// triangles of a chunk near the camera -- and the cascade pass fans one draw out to all four
		// cascades at once, so one level of detail has to serve the coarsest of them. Casting from the
		// full surface means several times the triangles for shadows that cannot record them.
		//
		// 1 keeps exactly the detail a shadow texel can hold, which measured barely cheaper than casting
		// from everything: the tree is deep where the camera is, and one texel of a near cascade is
		// small. 4 is two levels coarser -- shadows stop recording features under a couple of units,
		// which a four by four filtered shadow map was blurring away regardless, and the caster count
		// falls by an order of magnitude. This is the first number to raise when the shadow pass is
		// where the frame is going.
		float ShadowDetailBias = 4.0f;

		// Builds allowed in flight at once. Without a cap, a camera cut to the other side of the
		// planet queues every chunk it can see in one frame and the job system spends a second on
		// work that is already out of date.
		uint32_t MaxConcurrentBuilds = 8;

		// Keeps the triangles of leaf chunks in memory so collision can be cooked from them. Off by
		// default because it is tens of megabytes for a planet somebody is only looking at.
		bool KeepGeometryForCollision = false;

		friend bool operator==(const PlanetLodSettings&, const PlanetLodSettings&) = default;
	};

	// The quadtree over all six faces, and the only thing that decides what gets drawn.
	//
	// Lives on the PlanetComponent as runtime state -- it is not serialized, because it is entirely
	// derived from the settings that are. The update system drives it once a frame with the camera's
	// position; the render passes read the chunk list it leaves behind.
	class LEV_API PlanetSurface
	{
	public:
		PlanetSurface();

		// Settings in, tree out. Only what actually changed is thrown away: moving the LOD bias keeps
		// every mesh, changing the seed cannot keep any of them.
		void Configure(const PlanetShapeSettings& shape, const PlanetClimateSettings& climate,
		               const PlanetLodSettings& lod);

		//<--- Observer position in the planet's own space, which the system converts for us ---<<
		void Update(Vector3 observerPosition);

		//<--- Rebuilt every Update. Chunks are owned by the tree and outlive the list ---<<
		[[nodiscard]] const Vector<PlanetChunk*>& GetVisibleChunks() const { return m_VisibleChunks; }

		// The chunks worth casting shadows with: the coarsest level of the tree whose triangles are
		// still no larger than maxTriangleSize, in world units. A separate list from the one above,
		// because the surface is tessellated for the eye and a shadow map cannot record most of it --
		// see ShadowDetailBias.
		//
		// Filled rather than returned so the caller keeps the storage across frames. The shadow pass
		// is the only thing that knows what a shadow texel is worth, which is why it passes the size
		// in rather than reading it from here.
		void CollectShadowCasters(float maxTriangleSize, Vector<PlanetChunk*>& outChunks) const;

		[[nodiscard]] const Ref<PlanetSampler>& GetSampler() const { return m_Sampler; }
		[[nodiscard]] const PlanetLodSettings& GetLodSettings() const { return m_Lod; }

		[[nodiscard]] float GetRadius() const { return m_Shape.Radius; }

		//<--- Drops every mesh. The next Update regenerates what it needs ---<<
		void Invalidate();

		//<--- Statistics, for the editor's planet panel ---<<
		[[nodiscard]] uint32_t GetChunkCount() const { return m_ChunkCount; }
		[[nodiscard]] uint32_t GetVisibleChunkCount() const { return static_cast<uint32_t>(m_VisibleChunks.size()); }
		[[nodiscard]] uint32_t GetBuildingCount() const;
		[[nodiscard]] uint32_t GetDeepestVisibleDepth() const { return m_DeepestVisibleDepth; }

		//<--- Queries. Answered from the shape rather than from the meshes, so they work at any
		//distance and whether or not the ground has been generated ---<<
		[[nodiscard]] PlanetSurfaceSample SampleSurface(Vector3 direction) const;
		[[nodiscard]] float GetElevation(Vector3 direction) const;
		[[nodiscard]] float GetSurfaceRadius(Vector3 direction) const;

		// Where a ray from the planet's centre through direction meets the ground, in planet space.
		[[nodiscard]] Vector3 GetSurfacePoint(Vector3 direction) const;

	private:
		//<--- Returns true when this subtree has drawn itself, one way or another ---<<
		bool UpdateChunk(PlanetChunk& chunk, Vector3 observerPosition);

		void CollectShadowCastersFrom(const PlanetChunk& chunk, float maxTriangleSize,
		                              Vector<PlanetChunk*>& outChunks) const;

		[[nodiscard]] bool ShouldSplit(const PlanetChunk& chunk, Vector3 observerPosition) const;
		[[nodiscard]] bool CanStartBuild() const;

		void CreateRoots();

		PlanetShapeSettings m_Shape;
		PlanetClimateSettings m_Climate;
		PlanetLodSettings m_Lod;

		Ref<PlanetSampler> m_Sampler;

		Array<Scope<PlanetChunk>, k_PlanetFaceCount> m_Roots{};

		Vector<PlanetChunk*> m_VisibleChunks;

		// Shared with the jobs in flight, which is why it is a Ref: a job that outlives the planet
		// still has somewhere to report that it has finished.
		Ref<std::atomic<uint32_t>> m_BuildCounter;

		uint32_t m_ChunkCount = 0;
		uint32_t m_DeepestVisibleDepth = 0;

		bool m_IsConfigured = false;
	};
}
