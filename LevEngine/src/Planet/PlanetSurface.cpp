#include "levpch.h"
#include "PlanetSurface.h"

#include <EASTL/sort.h>

namespace LevEngine
{
	// How far the incidence term in GetTrianglePixels is allowed to coarsen a chunk: never below half
	// the detail the distance alone would ask for. Ground seen edge-on genuinely needs far fewer
	// triangles, but it is also where its own silhouette is drawn, and a floor is cheaper insurance
	// than any amount of cleverness about which chunks happen to be on a skyline.
	static constexpr float k_MinIncidenceScale = 0.5f;

	PlanetSurface::PlanetSurface()
		: m_BuildCounter(CreateRef<std::atomic<uint32_t>>(0)) { }

	void PlanetSurface::Configure(const PlanetShapeSettings& shape, const PlanetClimateSettings& climate,
	                             const PlanetLodSettings& lod)
	{
		// What has to be thrown away depends on what moved. The shape and the climate are baked into
		// vertices, so changing either invalidates every mesh; the chunk resolution changes what a
		// mesh is; the rest -- the LOD bias, the build budget -- only changes which chunks are wanted
		// next frame, and the ones already built are still correct.
		const bool shapeChanged = !(m_Shape == shape);
		const bool climateChanged = !(m_Climate == climate);
		const bool geometryChanged = m_Lod.ChunkResolution != lod.ChunkResolution
			|| m_Lod.SkirtDepthScale != lod.SkirtDepthScale;
		const bool treeChanged = m_Lod.MaxDepth != lod.MaxDepth;

		m_Shape = shape;
		m_Climate = climate;
		m_Lod = lod;

		if (!m_IsConfigured || shapeChanged || climateChanged)
			m_Sampler = CreateRef<PlanetSampler>(m_Shape, m_Climate);

		if (!m_IsConfigured || shapeChanged || climateChanged || geometryChanged || treeChanged)
		{
			m_IsConfigured = true;
			Invalidate();
		}
	}

	void PlanetSurface::CreateRoots()
	{
		const float elevationRange = m_Shape.GetElevationRange();

		for (uint32_t face = 0; face < k_PlanetFaceCount; ++face)
		{
			m_Roots[face] = CreateScope<PlanetChunk>(static_cast<PlanetFace>(face),
			                                         Vector2::Zero, Vector2::One, 0,
			                                         m_Shape.Radius, elevationRange);
		}
	}

	void PlanetSurface::Invalidate()
	{
		m_VisibleChunks.clear();

		for (Scope<PlanetChunk>& root : m_Roots)
			root.reset();

		CreateRoots();

		m_ChunkCount = 0;
		m_DeepestVisibleDepth = 0;
	}

	uint32_t PlanetSurface::GetBuildingCount() const
	{
		return m_BuildCounter ? m_BuildCounter->load(std::memory_order_relaxed) : 0;
	}

	bool PlanetSurface::CanStartBuild() const
	{
		return GetBuildingCount() < Math::Max(1u, m_Lod.MaxConcurrentBuilds);
	}

	float PlanetSurface::GetTrianglePixels(const PlanetChunk& chunk, const Vector3 observerPosition,
	                                      const float pixelsPerRadian) const
	{
		// Distance to the nearest point of the chunk, not to its middle. A chunk the camera is
		// standing on has its centre a long way off if it is a large one, and measuring to the centre
		// is what makes the ground under your feet refuse to subdivide.
		const float distanceToCenter = (observerPosition - chunk.GetBoundsCenter()).Length();
		const float distance = Math::Max(0.0f, distanceToCenter - chunk.GetBoundsRadius());

		//<--- Standing on it: as large on screen as it gets ---<<
		if (distance <= 0.0f) return std::numeric_limits<float>::max();

		// A chunk always has the same number of triangles, so one triangle is its width over its
		// resolution. That subtends triangle/distance radians at the eye, and a radian covers
		// pixelsPerRadian pixels, so this is how many pixels across the triangle really is.
		const float triangleSize = chunk.GetSize()
			/ static_cast<float>(Math::Max(1u, m_Lod.ChunkResolution));

		float pixels = triangleSize / distance * pixelsPerRadian;

		// How square-on the ground is to the eye, which the line above quietly assumed was always
		// fully. It is not, and standing on a planet is the case where it is least true: looking out
		// towards the horizon, the ground runs away from you almost edge-on, and a patch that is
		// triangleSize across covers a fraction of the pixels its distance alone suggests. Measured at
		// a hundred units up looking along the surface, that came to six hundred thousand triangles
		// over nine hundred thousand pixels -- one and a half pixels a triangle, against a target of
		// six. The metric was not being missed by a little; it was measuring the wrong thing.
		//
		// Screen area falls with the cosine of the incidence, and this is a length rather than an area,
		// hence the square root.
		if (distanceToCenter > Math::FloatEpsilon)
		{
			const Vector3 viewDirection = (chunk.GetBoundsCenter() - observerPosition) / distanceToCenter;

			// The sphere's normal, not the ground's. A cliff face inside the chunk can point somewhere
			// quite different, which is exactly why the floor below is not zero: at a grazing angle the
			// silhouette is the one thing the eye does judge geometry by, and this may not reduce
			// detail so far that a skyline goes blocky. Half a triangle of leeway, no more.
			const float incidence = std::abs(viewDirection.Dot(chunk.GetCenterDirection()));

			pixels *= Math::Max(std::sqrt(incidence), k_MinIncidenceScale);
		}

		return pixels;
	}

	float PlanetSurface::GetHorizonClearance(const PlanetChunk& chunk, const Vector3 observerPosition) const
	{
		// The occluder is the planet at its lowest, not at sea level. A sea level sphere would hide
		// the far wall of anything that dips below it, and a coastline seen across an ocean basin is
		// exactly that case -- so the sphere allowed to hide things is the one nothing can be under.
		const float occluderRadius = Math::Max(m_Shape.Radius + m_Shape.GetMinElevation(), 0.0f);
		const float occluderRadiusSq = occluderRadius * occluderRadius;

		const float observerDistanceSq = observerPosition.LengthSquared();

		//<--- Underground, or as good as. There is no horizon to be behind ---<<
		if (observerDistanceSq <= occluderRadiusSq) return std::numeric_limits<float>::max();

		// The horizon is the plane through the circle where the view rays graze the sphere, which sits
		// occluderRadius^2 / distance from the centre along the direction of the observer. This is how
		// far the chunk stands in front of it: its centre's clearance, widened by its bounding radius,
		// so a chunk counts as hidden only once the whole of it is.
		const float observerDistance = std::sqrt(observerDistanceSq);
		const float centerClearance =
			(observerPosition.Dot(chunk.GetBoundsCenter()) - occluderRadiusSq) / observerDistance;

		return centerClearance + chunk.GetBoundsRadius();
	}

	bool PlanetSurface::IsOutsideFrustum(const PlanetChunk& chunk) const
	{
		if (!m_Culling.ViewFrustum) return false;

		const Vector3 center = Vector3::Transform(chunk.GetBoundsCenter(), m_Culling.PlanetToWorld);
		const float radius = chunk.GetBoundsRadius() * m_Culling.WorldScale;

		const Frustum& frustum = *m_Culling.ViewFrustum;

		// Sides first: at any normal field of view they are what rejects, and the near and far planes
		// almost never do on a body that fills this much of the view.
		return frustum.leftFace.GetSignedDistanceToPlane(center) <= -radius
			|| frustum.rightFace.GetSignedDistanceToPlane(center) <= -radius
			|| frustum.topFace.GetSignedDistanceToPlane(center) <= -radius
			|| frustum.bottomFace.GetSignedDistanceToPlane(center) <= -radius
			|| frustum.nearFace.GetSignedDistanceToPlane(center) <= -radius
			|| frustum.farFace.GetSignedDistanceToPlane(center) <= -radius;
	}

	bool PlanetSurface::ShouldSplit(const PlanetChunk& chunk, const Vector3 observerPosition,
	                               const float pixelsPerRadian) const
	{
		if (chunk.GetDepth() >= m_Lod.MaxDepth) return false;

		return GetTrianglePixels(chunk, observerPosition, pixelsPerRadian)
			> Math::Max(0.25f, m_Lod.TargetTrianglePixels);
	}

	bool PlanetSurface::ShouldMerge(const PlanetChunk& chunk, const Vector3 observerPosition,
	                               const float pixelsPerRadian) const
	{
		// Half the split threshold by default. Between the two nothing changes, which is the whole
		// point: a camera drifting across the boundary keeps the meshes it already has.
		const float threshold = Math::Max(0.25f, m_Lod.TargetTrianglePixels)
			* Math::Clamp(m_Lod.MergeHysteresis, 0.05f, 1.0f);

		return GetTrianglePixels(chunk, observerPosition, pixelsPerRadian) < threshold;
	}

	bool PlanetSurface::TryUploadMesh(PlanetChunk& chunk)
	{
		if (!chunk.IsBuilding()) return false;

		//<--- Out of budget for this frame; the build waits rather than being thrown away ---<<
		if (m_UploadsThisFrame >= Math::Max(1u, m_Lod.MaxMeshUploadsPerFrame)) return false;

		const bool keepGeometry = m_Lod.KeepGeometryForCollision && !chunk.IsSplit();

		if (!chunk.TryFinishBuild(keepGeometry)) return false;

		++m_UploadsThisFrame;
		return true;
	}

	bool PlanetSurface::UpdateChunk(PlanetChunk& chunk, const Vector3 observerPosition,
	                               const float pixelsPerRadian)
	{
		//<--- Counted for the editor's statistics: every node the tree walked this frame ---<<
		++m_ChunkCount;

		//<--- Meshes appear here, on the main thread, whatever thread built the vertices ---<<
		TryUploadMesh(chunk);

		// Behind the planet. Nothing about where the camera is pointing can bring this back, and the
		// far side cannot cast a shadow onto the near side either, so this is the one cull allowed to
		// stop the tree growing. Returning true rather than false: the ground here does not need
		// covering by anything, so the parent must not draw itself on its account.
		const float horizonClearance = GetHorizonClearance(chunk, observerPosition);

		if (horizonClearance < 0.0f)
		{
			// Reclaimed once it is a full chunk past the horizon rather than the moment it crosses.
			// The line moves as the camera does, and merging exactly on it would throw four meshes
			// away and build them again every time it wobbled.
			if (chunk.IsSplit() && horizonClearance < -chunk.GetBoundsRadius())
				chunk.Merge();

			return true;
		}

		if (!chunk.IsReady() && !chunk.IsBuilding() && CanStartBuild())
		{
			chunk.RequestBuild(m_Sampler, m_Lod.ChunkResolution,
			                   chunk.GetSize() * m_Lod.SkirtDepthScale, m_BuildCounter);
		}

		// Split and merge are asked as two separate questions with two different thresholds, and
		// between them the answer to both is no -- which is what stops a chunk on the boundary from
		// rebuilding its children every frame as the camera drifts.
		if (ShouldSplit(chunk, observerPosition, pixelsPerRadian))
		{
			if (!chunk.IsSplit())
				chunk.Split(m_Shape.Radius, m_Shape.GetElevationRange());
		}
		else if (chunk.IsSplit() && chunk.IsReady()
			&& ShouldMerge(chunk, observerPosition, pixelsPerRadian))
		{
			// Only once this chunk can stand in for its children, or a camera pulling back faster than
			// the coarse mesh builds would open a hole.
			chunk.Merge();
		}

		if (chunk.IsSplit())
		{
			// Descend unconditionally, whether or not the children have meshes yet.
			//
			// This used to descend only into subtrees that were fully built, which quietly made the tree
			// unable to shrink: a chunk still waiting on a mesh stopped the walk, and everything below
			// it was never visited again, so it never got the chance to merge itself away. Moving the
			// camera then grew the tree without bound -- two thousand nodes and seventeen hundred draw
			// calls, climbing every second. Maintaining the tree and deciding what to draw are two
			// different jobs and only the second one cares whether a mesh has arrived.
			const size_t drawnMark = m_VisibleChunks.size();

			bool allDrawn = true;
			for (Scope<PlanetChunk>& child : chunk.GetChildren())
			{
				if (!child)
				{
					allDrawn = false;
					continue;
				}

				allDrawn &= UpdateChunk(*child, observerPosition, pixelsPerRadian);
			}

			if (allDrawn) return true;

			// Some of the ground below is not ready, so this level covers all of it instead. Whatever
			// the children did add is taken back off the list -- drawing both would overlap them, and
			// drawing only the ready ones would leave holes between.
			m_VisibleChunks.resize(drawnMark);
		}

		// Off screen. Only the draw list is affected: everything above ran exactly as if this were
		// visible, because a flick of the mouse is all it takes to want it back, and a tree that
		// unbuilds itself every time the camera turns costs far more than the memory it saves.
		if (IsOutsideFrustum(chunk)) return true;

		//<--- Nothing below could cover the ground, so this level stands in ---<<
		if (!chunk.IsReady()) return false;

		m_VisibleChunks.emplace_back(&chunk);
		m_DeepestVisibleDepth = Math::Max(m_DeepestVisibleDepth, chunk.GetDepth());

		return true;
	}

	void PlanetSurface::Update(const Vector3 observerPosition, const float pixelsPerRadian,
	                          const PlanetCullingView& culling)
	{
		LEV_PROFILE_FUNCTION();

		if (!m_IsConfigured || !m_Sampler) return;

		m_Culling = culling;

		m_VisibleChunks.clear();
		m_ChunkCount = 0;
		m_DeepestVisibleDepth = 0;
		m_UploadsThisFrame = 0;

		for (Scope<PlanetChunk>& root : m_Roots)
		{
			if (!root) continue;

			UpdateChunk(*root, observerPosition, pixelsPerRadian);
		}

		// Nearest first. The quadtree walk emits chunks face by face, which puts the ground behind a
		// mountain on screen before the mountain -- and every one of those pixels is then shaded twice,
		// through a pixel shader that writes four render targets. Drawing the near ones first lets the
		// depth test throw the hidden ones away before their shader runs, which is what
		// [earlydepthstencil] on the surface shader is for. Sorting a few hundred pointers costs
		// nothing next to a screen of overdraw.
		eastl::sort(m_VisibleChunks.begin(), m_VisibleChunks.end(),
		            [observerPosition](const PlanetChunk* left, const PlanetChunk* right)
		            {
			            return (left->GetBoundsCenter() - observerPosition).LengthSquared()
				            < (right->GetBoundsCenter() - observerPosition).LengthSquared();
		            });
	}

	void PlanetSurface::CollectShadowCastersFrom(const PlanetChunk& chunk, const float maxTriangleSize,
	                                            Vector<PlanetChunk*>& outChunks) const
	{
		const float triangleSize = chunk.GetSize() / static_cast<float>(Math::Max(1u, m_Lod.ChunkResolution));

		// Stop as soon as this level is fine enough for a shadow texel, and stop regardless if there is
		// nothing finer to descend into. A node whose children are still being built is drawn itself,
		// the same rule the visible list follows -- a hole in a shadow map is a stripe of light across
		// the ground.
		if (triangleSize <= maxTriangleSize || !chunk.IsSplit() || !chunk.AreChildrenReady())
		{
			if (chunk.IsReady())
				outChunks.emplace_back(const_cast<PlanetChunk*>(&chunk));

			return;
		}

		for (const Scope<PlanetChunk>& child : chunk.GetChildren())
		{
			if (!child) continue;

			CollectShadowCastersFrom(*child, maxTriangleSize, outChunks);
		}
	}

	void PlanetSurface::CollectShadowCasters(const float maxTriangleSize, Vector<PlanetChunk*>& outChunks) const
	{
		LEV_PROFILE_FUNCTION();

		outChunks.clear();

		//<--- Scaled by the bias here, so every caller gets the same interpretation of it ---<<
		const float triangleSize = maxTriangleSize * Math::Max(0.01f, m_Lod.ShadowDetailBias);

		for (const Scope<PlanetChunk>& root : m_Roots)
		{
			if (!root) continue;

			CollectShadowCastersFrom(*root, triangleSize, outChunks);
		}
	}

	PlanetSurfaceSample PlanetSurface::SampleSurface(const Vector3 direction) const
	{
		if (!m_Sampler) return {};

		Vector3 normalized = direction;
		normalized.Normalize();

		return m_Sampler->Sample(normalized);
	}

	float PlanetSurface::GetElevation(const Vector3 direction) const
	{
		if (!m_Sampler) return 0.0f;

		Vector3 normalized = direction;
		normalized.Normalize();

		return m_Sampler->GetElevation(normalized);
	}

	float PlanetSurface::GetSurfaceRadius(const Vector3 direction) const
	{
		return m_Shape.Radius + GetElevation(direction);
	}

	Vector3 PlanetSurface::GetSurfacePoint(const Vector3 direction) const
	{
		Vector3 normalized = direction;
		normalized.Normalize();

		return normalized * GetSurfaceRadius(normalized);
	}
}
