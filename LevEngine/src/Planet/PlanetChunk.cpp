#include "levpch.h"
#include "PlanetChunk.h"

#include "PlanetSampler.h"
#include "Renderer/3D/Mesh.h"
#include "VGJS.h"

namespace LevEngine
{
	PlanetChunk::PlanetChunk(const PlanetFace face, const Vector2 minCoordinates, const Vector2 maxCoordinates,
	                         const uint32_t depth, const float planetRadius, const float elevationRange)
		: m_Face(face)
		, m_Depth(depth)
		, m_MinCoordinates(minCoordinates)
		, m_MaxCoordinates(maxCoordinates)
	{
		const Vector2 center = (minCoordinates + maxCoordinates) * 0.5f;
		m_CenterDirection = PlanetGeometry::FaceToDirection(face, center);

		//<--- Corner directions, which give both the size and the bounding sphere ---<<
		const Vector3 corners[4] =
		{
			PlanetGeometry::FaceToDirection(face, minCoordinates) * planetRadius,
			PlanetGeometry::FaceToDirection(face, Vector2(maxCoordinates.x, minCoordinates.y)) * planetRadius,
			PlanetGeometry::FaceToDirection(face, Vector2(minCoordinates.x, maxCoordinates.y)) * planetRadius,
			PlanetGeometry::FaceToDirection(face, maxCoordinates) * planetRadius,
		};

		m_Size = Math::Max((corners[1] - corners[0]).Length(), (corners[2] - corners[0]).Length());

		m_BoundsCenter = m_CenterDirection * planetRadius;

		float boundsRadius = 0.0f;
		for (const Vector3& corner : corners)
			boundsRadius = Math::Max(boundsRadius, (corner - m_BoundsCenter).Length());

		//<--- Plus everything the shape could add or subtract, since it has not been sampled yet ---<<
		m_BoundsRadius = boundsRadius + elevationRange;
	}

	void PlanetChunk::Split(const float planetRadius, const float elevationRange)
	{
		if (IsSplit()) return;

		const Vector2 center = (m_MinCoordinates + m_MaxCoordinates) * 0.5f;

		const Vector2 childMinimums[4] =
		{
			m_MinCoordinates,
			Vector2(center.x, m_MinCoordinates.y),
			Vector2(m_MinCoordinates.x, center.y),
			center,
		};

		const Vector2 childMaximums[4] =
		{
			center,
			Vector2(m_MaxCoordinates.x, center.y),
			Vector2(center.x, m_MaxCoordinates.y),
			m_MaxCoordinates,
		};

		for (size_t child = 0; child < 4; ++child)
		{
			m_Children[child] = CreateScope<PlanetChunk>(m_Face, childMinimums[child], childMaximums[child],
			                                             m_Depth + 1, planetRadius, elevationRange);
		}
	}

	void PlanetChunk::Merge()
	{
		for (Scope<PlanetChunk>& child : m_Children)
			child.reset();
	}

	bool PlanetChunk::AreChildrenReady() const
	{
		if (!IsSplit()) return false;

		for (const Scope<PlanetChunk>& child : m_Children)
		{
			if (!child || !child->IsReady()) return false;
		}

		return true;
	}

	void PlanetChunk::RequestBuild(const Ref<PlanetSampler>& sampler, const uint32_t resolution,
	                              const float skirtDepth, const Ref<std::atomic<uint32_t>>& buildCounter)
	{
		if (m_Build || m_Mesh) return;

		m_Build = CreateRef<PlanetChunkBuild>();
		m_BuildCounter = buildCounter;

		if (buildCounter)
			buildCounter->fetch_add(1, std::memory_order_relaxed);

		// Everything the job needs is captured by value. It cannot reach this chunk, which is the
		// point: a chunk merged away mid-build leaves the worker filling a PlanetChunkBuild that
		// nobody will read, and that is a wasted job rather than a crash.
		const Ref<PlanetChunkBuild> build = m_Build;
		const PlanetFace face = m_Face;
		const Vector2 minCoordinates = m_MinCoordinates;
		const Vector2 maxCoordinates = m_MaxCoordinates;

		vgjs::schedule([build, sampler, buildCounter, face, minCoordinates, maxCoordinates, resolution, skirtDepth]
		{
			try
			{
				build->Geometry = PlanetChunkBuilder::Build(*sampler, face, minCoordinates, maxCoordinates,
				                                           resolution, skirtDepth);
			}
			catch (std::exception& e)
			{
				Log::CoreError("Failed to build a planet chunk: '{}'", e.what());
			}

			// Release after the geometry is written, so a main thread that sees the flag sees the
			// geometry with it. The counter is only a budget and can be decremented either side.
			build->IsDone.store(true, std::memory_order_release);

			if (buildCounter)
				buildCounter->fetch_sub(1, std::memory_order_relaxed);
		});
	}

	bool PlanetChunk::TryFinishBuild(const bool keepGeometry)
	{
		if (!m_Build) return false;
		if (!m_Build->IsDone.load(std::memory_order_acquire)) return false;

		const PlanetChunkGeometry& geometry = m_Build->Geometry;

		m_Mesh = PlanetChunkBuilder::CreateMesh(geometry);
		m_Origin = geometry.Origin;
		m_MinElevation = geometry.MinElevation;
		m_MaxElevation = geometry.MaxElevation;

		//<--- The real bounds, now that the ground has been sampled instead of guessed at ---<<
		m_BoundsCenter = geometry.Origin;
		m_BoundsRadius = geometry.BoundingRadius;

		if (keepGeometry)
		{
			// Collision needs the triangles again to cook a PhysX mesh out of them, and cooking is
			// not something to do on the frame the chunk appears. Only the positions and indices are
			// kept -- the shading streams are already in GPU buffers and nothing reads them twice.
			m_Geometry = CreateScope<PlanetChunkGeometry>();
			m_Geometry->Origin = geometry.Origin;
			m_Geometry->Positions = geometry.Positions;
			m_Geometry->Indices = geometry.Indices;
			m_Geometry->BoundingRadius = geometry.BoundingRadius;
			m_Geometry->MinElevation = geometry.MinElevation;
			m_Geometry->MaxElevation = geometry.MaxElevation;
		}

		m_Build.reset();
		m_BuildCounter.reset();

		return m_Mesh != nullptr;
	}

	void PlanetChunk::Invalidate()
	{
		m_Mesh.reset();
		m_Geometry.reset();
		m_Build.reset();
		m_BuildCounter.reset();

		Merge();
	}
}
