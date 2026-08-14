#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"
#include "Math/Noise/Noise.h"

namespace LevEngine
{
	// What the ground is shaped like, as a function from a direction to a height above sea level.
	//
	// Heights are world units and are signed: zero is sea level, negative is sea floor. Nothing here
	// knows about meshes or chunks -- a direction goes in, a height comes out -- which is what lets
	// the same function serve the mesh builder, collision cooking, a spawn query and the editor's
	// preview without any of them agreeing on a resolution first.
	struct LEV_API PlanetShapeSettings
	{
		uint32_t Seed = 1337;

		//<--- Sea level radius, in world units. The mesh is this plus GetElevation ---<<
		float Radius = 4000.0f;

		//<--- Continents ---<<

		// The field that decides land from sea. Its frequency is how many continents there are:
		// around 2 gives a handful of large masses, 6 gives an archipelago.
		Noise::FractalSettings Continents{2.0f, 5, 2.03f, 0.5f, 0};

		// Continent noise is symmetric, so its coastlines are the level sets of a blobby field --
		// smooth ovals. Warping the domain first folds those ovals into bays, headlands and
		// island chains, which is most of what makes a coast read as a coast.
		float ContinentWarp = 0.4f;
		float ContinentWarpFrequency = 1.5f;

		// Where in the continent field the shoreline sits. This is the land/water ratio and nothing
		// else: 0 is about half land, positive values drown it, negative values dry it out. Earth is
		// around 0.25.
		float SeaLevelThreshold = 0.25f;

		// How much of the continent field is spent climbing out of the water. Small values give
		// cliffs straight out of the sea, large ones give wide continental shelves and beaches --
		// this is the number that decides whether there is anywhere for a beach to be.
		float ShoreWidth = 0.12f;

		// Depth of the deepest ocean and the height of unremarkable dry land.
		//
		// These are a fraction of the radius, and a small one: Earth's deepest trench is a sixth of a
		// percent of its radius and Everest less than a seventh. Nothing here is that modest -- a
		// planet small enough to walk across needs its relief exaggerated to read as relief at all --
		// but the ratio still has to stay well under a tenth, or the ridged mountain noise turns the
		// surface into a ball of spikes rather than a world with ranges on it.
		float OceanDepth = 220.0f;
		float LandHeight = 90.0f;

		//<--- Mountains ---<<

		// Ridged noise, which is what makes a range rather than a lump. See Noise::Ridged.
		Noise::FractalSettings Mountains{5.0f, 7, 2.03f, 0.5f, 0};

		// Roughly a tenth of the default radius, which is already ten times Earth's proportions. Push
		// it much past this and the highest octaves of the ridged sum stop being crags and start being
		// needles -- and needles narrower than the vertex spacing alias into a different set of
		// needles every time the level of detail changes.
		float MountainHeight = 380.0f;

		// Mountains do not cover continents evenly -- they run in belts, because that is where
		// plates meet. A separate low frequency field folded about its own zeros gives belts of
		// roughly the right width and spacing without simulating any plates.
		Noise::FractalSettings MountainBelts{1.7f, 3, 2.03f, 0.5f, 0};
		float MountainBeltWidth = 0.35f;

		// Ranges stop at the coast rather than rising out of the sea, so the belt is multiplied by
		// how far inland the point is. Islands are the exception and get their own arcs from the
		// belt field crossing water.
		float MountainInlandBias = 0.6f;

		//<--- Hills and roughness, the scale a vertex can still resolve ---<<
		Noise::FractalSettings Detail{24.0f, 4, 2.03f, 0.5f, 0};
		float DetailHeight = 14.0f;

		// Detail on a mountainside is scree and outcrop; the same amplitude on a plain is sandpaper.
		// Scaling it by how much mountain is already there keeps plains flat and ridges rough. Relief
		// stands in for slope here because the mountain term is already computed and a real gradient
		// would cost another two elevation samples per vertex.
		float DetailReliefBias = 0.7f;

		//<--- Longest length in the shape: mountains on top of land on top of the shelf ---<<
		[[nodiscard]] float GetMaxElevation() const { return LandHeight + MountainHeight + DetailHeight; }
		[[nodiscard]] float GetMinElevation() const { return -(OceanDepth + DetailHeight); }

		//<--- The widest the surface can stray from the sea level sphere, either way ---<<
		[[nodiscard]] float GetElevationRange() const
		{
			return Math::Max(GetMaxElevation(), -GetMinElevation());
		}

		friend bool operator==(const PlanetShapeSettings&, const PlanetShapeSettings&) = default;
	};

	// Everything the shape knows about one point, kept together because the climate model needs the
	// intermediate values and recomputing them would double the noise cost of a vertex.
	struct PlanetShapeSample
	{
		//<--- Signed height above sea level, world units ---<<
		float Elevation = 0.0f;

		//<--- The raw continent field, before the shoreline threshold. Ocean is below it ---<<
		float Continent = 0.0f;

		//<--- 0 at the waterline, 1 once fully ashore. This is what a beach lives inside ---<<
		float LandMask = 0.0f;

		// How far into a landmass the point is, from the continent field at continental scale. A
		// coast is 0 and the middle of a large mass approaches 1. Rainfall depends on this more
		// than on anything else, which is why the shape hands it to the climate rather than the
		// climate guessing.
		float Continentality = 0.0f;
	};

	class LEV_API PlanetShape
	{
	public:
		PlanetShape() = default;
		explicit PlanetShape(const PlanetShapeSettings& settings) : m_Settings(settings) { }

		[[nodiscard]] const PlanetShapeSettings& GetSettings() const { return m_Settings; }
		void SetSettings(const PlanetShapeSettings& settings) { m_Settings = settings; }

		//<--- Direction must be normalized. Everything else here goes through this ---<<
		[[nodiscard]] PlanetShapeSample Sample(Vector3 direction) const;

		[[nodiscard]] float GetElevation(const Vector3 direction) const { return Sample(direction).Elevation; }

		//<--- Distance from the planet's centre to the surface along direction ---<<
		[[nodiscard]] float GetSurfaceRadius(const Vector3 direction) const
		{
			return m_Settings.Radius + GetElevation(direction);
		}

		[[nodiscard]] Vector3 GetSurfacePoint(const Vector3 direction) const
		{
			return direction * GetSurfaceRadius(direction);
		}

		// Surface normal by finite difference along two tangents. Only for callers that want a
		// normal at a single point -- the mesh builder gets its normals from the triangles it has
		// already built, which is both cheaper and consistent with the geometry that gets drawn.
		// Spacing is an arc length in world units; too small and it reads floating point noise.
		[[nodiscard]] Vector3 GetSurfaceNormal(Vector3 direction, float spacing = 1.0f) const;

	private:
		PlanetShapeSettings m_Settings;
	};
}
