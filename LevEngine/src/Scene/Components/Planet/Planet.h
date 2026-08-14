#pragma once

#include "Kernel/Core.h"
#include "Math/Color.h"
#include "Planet/PlanetBiome.h"
#include "Planet/PlanetClimate.h"
#include "Planet/PlanetShape.h"
#include "Planet/PlanetSurface.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	class PlanetBiomeSetAsset;
	class PlanetCollision;

	// The sea. It has no shape of its own -- it is the sphere at sea level, and where the ground is
	// above that there is no water. See PlanetOcean.hlsl for why it needs no geometry.
	struct LEV_API PlanetOceanSettings
	{
		bool Render = true;

		// Water is not blue. It is nearly colourless in a glass and blue in a fjord, because the
		// colour comes from how much of it the light goes through -- which is why there are two of
		// these and a distance over which one becomes the other.
		Color ShallowColor = Color(0.22f, 0.55f, 0.6f, 1.0f);
		Color DeepColor = Color(0.02f, 0.09f, 0.19f, 1.0f);

		//<--- Depth, in world units, at which the water reads as fully deep ---<<
		float DepthFalloff = 120.0f;

		float Roughness = 0.06f;
		float Opacity = 0.85f;

		//<--- How much the grazing-angle mirror adds to that opacity ---<<
		float FresnelStrength = 0.9f;

		//<--- Waves bend the normal rather than move the surface: see PlanetOcean.hlsl ---<<
		float WaveStrength = 0.35f;
		float WaveScale = 0.6f;
		float WaveSpeed = 0.15f;

		friend bool operator==(const PlanetOceanSettings&, const PlanetOceanSettings&) = default;
	};

	// Detail finer than a vertex, added by the surface shader from the same noise the mesh was built
	// with. This is the only thing the two implementations of the noise exist for.
	struct LEV_API PlanetSurfaceDetailSettings
	{
		//<--- How far the per-pixel normal is bent. 0 turns the shader-side detail off ---<<
		float Strength = 0.6f;
		float Frequency = 2.5f;

		// Sharpness of the triplanar blend. Ground textures are projected along all three axes and
		// blended by the normal, because a sphere has no seamless UV mapping.
		float TriplanarSharpness = 6.0f;

		//<--- Distances over which the detail fades out, in world units ---<<
		float FadeStart = 120.0f;
		float FadeEnd = 400.0f;

		friend bool operator==(const PlanetSurfaceDetailSettings&, const PlanetSurfaceDetailSettings&) = default;
	};

	enum class PlanetPreset
	{
		Earthlike = 0,
		Desert = 1,
		IceWorld = 2,
		OceanWorld = 3,
		Volcanic = 4,
		Custom = 5,
	};

	// A generated planet. Sits on an entity with a Transform, whose position is the planet's centre
	// and whose rotation turns the whole world -- so an AtmosphereComponent on the same entity with a
	// matching PlanetRadius gives it a sky, and the two agree about where the ground is.
	//
	// Everything here is settings. The ground itself is in Surface, which is runtime state: it is not
	// serialized, because it can always be rebuilt from the seed, and a saved copy of a few hundred
	// megabytes of vertices would go stale the moment a slider moved.
	REGISTER_PARSE_TYPE(PlanetComponent);
	struct LEV_API PlanetComponent
	{
		PlanetPreset Preset = PlanetPreset::Earthlike;

		PlanetShapeSettings Shape;
		PlanetClimateSettings Climate;
		PlanetLodSettings Lod;
		PlanetOceanSettings Ocean;
		PlanetSurfaceDetailSettings Detail;

		// The biomes and the ground textures. Without one the planet still generates and still has
		// climate; it is drawn with the default Earthlike biome tints instead, which is enough to see
		// the continents, the deserts and the ice caps.
		Ref<PlanetBiomeSetAsset> BiomeSet;

		//<--- Collision is cooked for the chunks near whatever is standing on the planet ---<<
		bool GenerateCollision = false;

		// How far around each body on the surface to keep colliders, in world units. Cooking a PhysX
		// mesh is not cheap and a planet has far too many chunks to cook them all, so only the ground
		// something could actually fall onto is given any.
		float CollisionRadius = 400.0f;

		// The quadtree, the meshes and the sampler. Shared rather than owned by value because entt
		// moves components around and the passes hold on to it across a frame.
		Ref<PlanetSurface> Surface;

		// The PhysX actors for the ground near anything standing on it. Created by
		// PlanetCollisionSystem the first time it is needed, so a planet nobody walks on never pays
		// for one. Runtime state, like Surface.
		Ref<PlanetCollision> Collision;

		PlanetComponent();
		PlanetComponent(const PlanetComponent&) = default;

		//<--- Pushes the settings into the surface, rebuilding only what changed ---<<
		void ApplySettings() const;

		[[nodiscard]] const Vector<PlanetBiome>& GetBiomes() const;
	};

	struct LEV_API PlanetPresets
	{
		static void Apply(PlanetComponent& component, PlanetPreset preset);
		static const char* GetName(PlanetPreset preset);
	};
}
