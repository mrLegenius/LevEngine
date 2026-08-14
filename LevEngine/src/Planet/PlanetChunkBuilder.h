#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"
#include "PlanetGeometry.h"

namespace LevEngine
{
	class Mesh;
	class PlanetSampler;

	// The vertex data of one chunk, in a form that can be produced off the main thread.
	//
	// Split from the Mesh on purpose: building a Mesh means creating GPU buffers, and the render
	// device is not something several worker threads can be let at. So a job fills one of these --
	// plain arrays, no device calls -- and the main thread turns it into a Mesh when it is done.
	struct PlanetChunkGeometry
	{
		// Positions are relative to Origin, not to the planet's centre. At a radius of a few million
		// units a float has half a unit of precision left, which is a visibly shaking surface; the
		// same float measured from the middle of a chunk a few hundred units across has millimetres.
		// The chunk's model matrix carries Origin, so nothing downstream has to know.
		Vector3 Origin = Vector3::Zero;

		Vector<Vector3> Positions;
		Vector<Vector3> Normals;
		Vector<Vector3> Tangents;
		Vector<Vector2> UVs;

		// Per-vertex (elevation above sea level, temperature in Celsius, humidity 0..1, slope 0..1).
		// The climate is evaluated here, once per vertex, and the shader interpolates it -- rather
		// than being evaluated per pixel, which would mean the pixel shader running the whole noise
		// stack including the rain shadow's second elevation sample.
		Vector<Vector4> Climate;

		Vector<uint32_t> Indices;

		//<--- Bounding sphere around Origin, for culling and for the LOD error metric ---<<
		float BoundingRadius = 0.0f;

		float MinElevation = 0.0f;
		float MaxElevation = 0.0f;

		//<--- Whether any of it is above sea level, which is all the ocean pass needs to know ---<<
		[[nodiscard]] bool HasLand() const { return MaxElevation > 0.0f; }
		[[nodiscard]] bool HasWater() const { return MinElevation < 0.0f; }
	};

	struct LEV_API PlanetChunkBuilder
	{
		// Builds the grid covering [minCoordinates, maxCoordinates] of a cube face.
		//
		// resolution is quads per side, so the grid has (resolution + 1) squared vertices. Every
		// chunk at every depth uses the same resolution -- that is what makes the quadtree work:
		// splitting a chunk into four doubles the vertex density over the same ground.
		//
		// skirtDepth is how far the border wall hangs below the surface, in world units. Two chunks
		// at different depths do not agree along their shared edge -- the coarse one interpolates
		// across ground the fine one resolves -- so there is a crack, and you can see the sky through
		// it. A wall hanging down from the edge of every chunk fills it. Cheaper and more robust than
		// stitching the two edges together, which needs each chunk to know its neighbours' depths and
		// to have a mesh variant for every combination of them.
		[[nodiscard]] static PlanetChunkGeometry Build(const PlanetSampler& sampler, PlanetFace face,
		                                              Vector2 minCoordinates, Vector2 maxCoordinates,
		                                              uint32_t resolution, float skirtDepth);

		//<--- Main thread only: this creates GPU buffers ---<<
		[[nodiscard]] static Ref<Mesh> CreateMesh(const PlanetChunkGeometry& geometry);

		//<--- The semantic the climate stream is bound to. See PlanetSurface.hlsl ---<<
		static constexpr const char* k_ClimateSemantic = "CLIMATE";
	};
}
