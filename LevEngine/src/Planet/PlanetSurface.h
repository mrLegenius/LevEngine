#pragma once

#include "Kernel/Core.h"
#include "DataTypes/Array.h"
#include "Math/Frustum.h"
#include "PlanetChunk.h"
#include "PlanetSampler.h"

namespace LevEngine
{
	// What the walk needs in order to decide a chunk cannot be seen this frame.
	//
	// There are two kinds of hidden here and they are deliberately not treated the same. Behind the
	// planet does not depend on where the camera is pointing, only on where it is, and the far side
	// cannot cast a shadow onto the near side either -- the planet is in the way -- so that one is
	// allowed to stop the tree growing and eventually reclaim what is already there. Outside the
	// frustum is one flick of the mouse away from being false, so it only takes chunks off the draw
	// list and leaves the tree exactly as it was.
	struct LEV_API PlanetCullingView
	{
		//<--- World space, from SceneCamera::GetFrustum. Null disables frustum culling ---<<
		const Frustum* ViewFrustum = nullptr;

		//<--- Chunk bounds are in planet space and the frustum is not, so the two have to meet ---<<
		Matrix PlanetToWorld;
		float WorldScale = 1.0f;
	};

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

		// How large a chunk's triangles may get on screen, in pixels, before it splits.
		//
		// This used to be a plain distance multiplier, which held triangles at a constant angle rather
		// than a constant size: it knew nothing about the field of view or the viewport, so what it
		// meant changed with both. Its default worked out to nearly a degree per triangle, which is ten
		// pixels and twenty just before a split -- visibly faceted, which is what it looked like.
		// Pixels are the unit the eye judges this in, so it is the unit to state it in.
		//
		// 3 is about as coarse as a silhouette gets without showing. Below 2 the triangle count climbs
		// as the square for detail nobody can see.
		float TargetTrianglePixels = 3.0f;

		// How far below the split threshold a chunk has to fall before its children are given back.
		//
		// Without this a chunk sitting exactly at the threshold splits and merges on alternating
		// frames, and each cycle throws away four meshes and builds them again -- which is why a moving
		// camera cost far more than a still one. Splitting at three pixels and merging at half that
		// leaves a band where whatever already exists is kept, and the thrash has nowhere to happen.
		float MergeHysteresis = 0.5f;

		// Chunks whose vertices may be handed to the GPU in one frame.
		//
		// Building a mesh happens on a worker, but creating its buffers is a driver allocation and has
		// to be on the thread that owns the device. Half a dozen buffers per chunk at tens of
		// microseconds each is a millisecond a frame if a burst of them lands together, and a burst is
		// exactly what a camera starting to move produces. Finished builds simply wait a frame.
		//
		// Generous on purpose. This was 2, which turned out to be far too tight: builds finished faster
		// than they could be uploaded, so there was always a backlog, and a chunk waiting on a mesh is
		// a chunk whose subtree cannot be reclaimed. The tree grew without bound while the camera moved.
		// A backlog is much more expensive than the hitch this is guarding against.
		uint32_t MaxMeshUploadsPerFrame = 8;

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

		// Observer position in the planet's own space, which the system converts for us, and how many
		// pixels one radian of the view covers -- the viewport's height over its vertical field of
		// view. That is what turns a triangle's angular size into a pixel count, and the render pass is
		// the only thing that knows it. The culling view is what the render pass knows about where the
		// camera is looking; a default constructed one draws the whole sphere, as this used to.
		void Update(Vector3 observerPosition, float pixelsPerRadian, const PlanetCullingView& culling = {});

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
		bool UpdateChunk(PlanetChunk& chunk, Vector3 observerPosition, float pixelsPerRadian);

		void CollectShadowCastersFrom(const PlanetChunk& chunk, float maxTriangleSize,
		                              Vector<PlanetChunk*>& outChunks) const;

		[[nodiscard]] bool ShouldSplit(const PlanetChunk& chunk, Vector3 observerPosition,
		                               float pixelsPerRadian) const;

		//<--- Deliberately not the negation of the above: see MergeHysteresis ---<<
		[[nodiscard]] bool ShouldMerge(const PlanetChunk& chunk, Vector3 observerPosition,
		                               float pixelsPerRadian) const;

		//<--- Size of a chunk's triangles on screen, in pixels, which both tests are built on ---<<
		[[nodiscard]] float GetTrianglePixels(const PlanetChunk& chunk, Vector3 observerPosition,
		                                      float pixelsPerRadian) const;

		// How far the nearest part of a chunk stands in front of the horizon plane, in world units.
		// Negative is behind the planet. Returned as a distance rather than a bool because the walk
		// wants two different amounts of it: any at all to draw, and a chunk's width to reclaim.
		[[nodiscard]] float GetHorizonClearance(const PlanetChunk& chunk, Vector3 observerPosition) const;

		//<--- Bounding sphere against the six planes. Always false without a frustum ---<<
		[[nodiscard]] bool IsOutsideFrustum(const PlanetChunk& chunk) const;

		//<--- Turns a finished build into a mesh, if this frame still has the budget for one ---<<
		bool TryUploadMesh(PlanetChunk& chunk);
		[[nodiscard]] bool CanStartBuild() const;

		void CreateRoots();

		PlanetShapeSettings m_Shape;
		PlanetClimateSettings m_Climate;
		PlanetLodSettings m_Lod;

		//<--- Set at the top of every Update, so the recursive walk does not have to carry it ---<<
		PlanetCullingView m_Culling;

		Ref<PlanetSampler> m_Sampler;

		Array<Scope<PlanetChunk>, k_PlanetFaceCount> m_Roots{};

		Vector<PlanetChunk*> m_VisibleChunks;

		// Shared with the jobs in flight, which is why it is a Ref: a job that outlives the planet
		// still has somewhere to report that it has finished.
		Ref<std::atomic<uint32_t>> m_BuildCounter;

		uint32_t m_ChunkCount = 0;
		uint32_t m_UploadsThisFrame = 0;
		uint32_t m_DeepestVisibleDepth = 0;

		bool m_IsConfigured = false;
	};
}
