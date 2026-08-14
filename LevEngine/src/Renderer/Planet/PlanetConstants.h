#pragma once

#include "Kernel/Core.h"
#include "Math/Math.h"
#include "Planet/PlanetBiome.h"

namespace LevEngine
{
	class ConstantBuffer;
	class SamplerState;
	class Texture;
	struct PlanetComponent;

	// Mirrors PlanetBiomeData in PlanetCommon.hlsli. Packed into float4s by hand, like the atmosphere's
	// buffer, because HLSL's own packing rules and C++'s do not agree about anything that is not.
	struct LEV_API GPUPlanetBiome
	{
		Vector4 ClimateRange{-80.0f, 80.0f, 0.0f, 1.0f}; //<--- tempMin, tempMax, humidityMin, humidityMax ---<<
		Vector4 Blends{4.0f, 0.1f, 60.0f, 0.1f};         //<--- temperature, humidity, height, slope ---<<
		Vector4 HeightSlope{-100000.0f, 100000.0f, 0.0f, 1.0f};
		Vector4 Tint{1.0f, 1.0f, 1.0f, 1.0f};            //<--- rgb, priority ---<<
		Vector4 Surface{0.9f, 0.0f, 12.0f, 0.0f};        //<--- roughness, metallic, textureScale, slice ---<<
		Vector4 Detail{1.0f, 0.0f, 0.0f, 0.0f};          //<--- normalStrength ---<<
	};

	// Mirrors PlanetConstantBuffer in PlanetCommon.hlsli. Keep the two in step.
	struct LEV_API alignas(16) GPUPlanetData
	{
		float SurfaceRadius = 1.0f;
		float ElevationRange = 1.0f;
		int32_t BiomeCount = 0;
		int32_t TextureFlags = 0;

		float DetailStrength = 0.0f;
		float DetailFrequency = 1.0f;
		uint32_t DetailSeed = 0;
		float TriplanarSharpness = 6.0f;

		float DetailFadeStart = 100.0f;
		float DetailFadeEnd = 400.0f;
		float Time = 0.0f;
		float OceanOpacity = 1.0f;

		Vector4 OceanShallowColor{0.2f, 0.5f, 0.6f, 1.0f};
		Vector4 OceanDeepColor{0.02f, 0.09f, 0.19f, 1.0f};

		float OceanDepthFalloff = 100.0f;
		float OceanRoughness = 0.05f;
		float WaveStrength = 0.0f;
		float WaveScale = 1.0f;

		float WaveSpeed = 0.0f;
		float OceanFresnel = 1.0f;
		Vector2 Padding{};

		//<--- The planet's transform and its inverse, so a shader can move between the two spaces ---<<
		Matrix PlanetToWorld;
		Matrix WorldToPlanet;

		GPUPlanetBiome Biomes[k_MaxPlanetBiomes]{};
	};

	// Fills and binds the two planet constant buffers, and binds the ground texture arrays.
	//
	// Shared by the surface pass, the ocean pass and the shadow pass rather than owned by any of
	// them: all three draw the same chunks of the same planet and none is the natural owner of the
	// numbers describing it.
	class LEV_API PlanetConstants
	{
	public:
		PlanetConstants();

		// Packs the component into the buffer and binds it. Textures come with it, and so do the
		// planet's transforms -- which is everything the shaders need, so nothing is rebound per chunk.
		void Bind(PlanetComponent& planet, const Matrix& planetToWorld);

		void Unbind();

	private:
		void BindTextures(PlanetComponent& planet);

		Ref<ConstantBuffer> m_PlanetBuffer;

		//<--- One sampler for all three arrays: they are sampled at the same coordinates ---<<
		Ref<SamplerState> m_GroundSampler;

		GPUPlanetData m_Data;

		//<--- Held so Unbind can release exactly what was bound ---<<
		Ref<Texture> m_AlbedoArray;
		Ref<Texture> m_NormalArray;
		Ref<Texture> m_RoughnessArray;
	};
}
