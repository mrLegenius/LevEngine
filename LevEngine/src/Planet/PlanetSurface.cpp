#include "levpch.h"
#include "PlanetSurface.h"

#include <EASTL/sort.h>

namespace LevEngine
{
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

	bool PlanetSurface::ShouldSplit(const PlanetChunk& chunk, const Vector3 observerPosition) const
	{
		if (chunk.GetDepth() >= m_Lod.MaxDepth) return false;

		// Distance to the nearest point of the chunk, not to its middle. A chunk the camera is
		// standing on has its centre a long way off if it is a large one, and measuring to the centre
		// is what makes the ground under your feet refuse to subdivide.
		const float distanceToCenter = (observerPosition - chunk.GetBoundsCenter()).Length();
		const float distance = Math::Max(0.0f, distanceToCenter - chunk.GetBoundsRadius());

		// A chunk always has the same number of triangles, so its triangles are its size over its
		// resolution. Splitting when the distance falls under the chunk's own width times a constant
		// keeps that triangle size roughly constant in screen space, which is the thing the eye
		// actually notices.
		return distance < chunk.GetSize() * Math::Max(0.1f, m_Lod.LodBias);
	}

	bool PlanetSurface::UpdateChunk(PlanetChunk& chunk, const Vector3 observerPosition)
	{
		//<--- Counted for the editor's statistics: every node the tree walked this frame ---<<
		++m_ChunkCount;

		//<--- Meshes appear here, on the main thread, whatever thread built the vertices ---<<
		const bool keepGeometry = m_Lod.KeepGeometryForCollision && !chunk.IsSplit();
		chunk.TryFinishBuild(keepGeometry);

		if (!chunk.IsReady() && !chunk.IsBuilding() && CanStartBuild())
		{
			chunk.RequestBuild(m_Sampler, m_Lod.ChunkResolution,
			                   chunk.GetSize() * m_Lod.SkirtDepthScale, m_BuildCounter);
		}

		const bool wantsSplit = ShouldSplit(chunk, observerPosition);

		if (!wantsSplit)
		{
			// Merging only once this chunk can stand in for its children keeps a hole from opening
			// when the camera pulls back faster than the coarse mesh can be built.
			if (chunk.IsSplit() && chunk.IsReady())
				chunk.Merge();

			if (!chunk.IsReady()) return false;

			m_VisibleChunks.emplace_back(&chunk);
			m_DeepestVisibleDepth = Math::Max(m_DeepestVisibleDepth, chunk.GetDepth());

			return true;
		}

		if (!chunk.IsSplit())
			chunk.Split(m_Shape.Radius, m_Shape.GetElevationRange());

		// The children are visited whether or not they are all ready, because visiting them is what
		// starts their builds. Only when all four have a mesh does this chunk stop being drawn --
		// otherwise the surface would show a hole where a child is still being generated.
		const bool childrenReady = chunk.AreChildrenReady();

		if (!childrenReady)
		{
			for (Scope<PlanetChunk>& child : chunk.GetChildren())
			{
				if (!child) continue;

				child->TryFinishBuild(m_Lod.KeepGeometryForCollision && !child->IsSplit());

				if (!child->IsReady() && !child->IsBuilding() && CanStartBuild())
				{
					child->RequestBuild(m_Sampler, m_Lod.ChunkResolution,
					                    child->GetSize() * m_Lod.SkirtDepthScale, m_BuildCounter);
				}
			}

			if (!chunk.IsReady()) return false;

			m_VisibleChunks.emplace_back(&chunk);
			m_DeepestVisibleDepth = Math::Max(m_DeepestVisibleDepth, chunk.GetDepth());

			return true;
		}

		bool anyDrawn = false;
		for (Scope<PlanetChunk>& child : chunk.GetChildren())
		{
			if (!child) continue;

			anyDrawn |= UpdateChunk(*child, observerPosition);
		}

		if (anyDrawn) return true;

		//<--- Nothing below could draw itself after all, so fall back to this level ---<<
		if (!chunk.IsReady()) return false;

		m_VisibleChunks.emplace_back(&chunk);

		return true;
	}

	void PlanetSurface::Update(const Vector3 observerPosition)
	{
		LEV_PROFILE_FUNCTION();

		if (!m_IsConfigured || !m_Sampler) return;

		m_VisibleChunks.clear();
		m_ChunkCount = 0;
		m_DeepestVisibleDepth = 0;

		for (Scope<PlanetChunk>& root : m_Roots)
		{
			if (!root) continue;

			UpdateChunk(*root, observerPosition);
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
