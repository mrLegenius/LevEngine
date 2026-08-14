#include "levpch.h"
#include "PlanetConstants.h"

#include "Assets/PlanetBiomeSetAsset.h"
#include "Kernel/Time/Time.h"
#include "Renderer/Pipeline/ConstantBuffer.h"
#include "Renderer/Pipeline/SamplerState.h"
#include "Renderer/Pipeline/Texture.h"
#include "Renderer/Shader/ShaderType.h"
#include "Scene/Components/Planet/Planet.h"

namespace LevEngine
{
	namespace
	{
		//<--- CB_PLANET and CB_PLANET_CHUNK in Registers.hlsli ---<<
		constexpr uint32_t k_PlanetConstantBufferSlot = 11;

		//<--- T_PLANET_ALBEDO and the two after it, and S_PLANET_GROUND ---<<
		constexpr uint32_t k_AlbedoSlot = 0;
		constexpr uint32_t k_NormalSlot = 1;
		constexpr uint32_t k_RoughnessSlot = 2;
		constexpr uint32_t k_GroundSamplerSlot = 0;

		//<--- Bits of GPUPlanetData::TextureFlags, mirroring PLANET_HAS_* in PlanetCommon.hlsli ---<<
		constexpr int32_t k_HasAlbedo = 1;
		constexpr int32_t k_HasNormal = 2;
		constexpr int32_t k_HasRoughness = 4;
	}

	PlanetConstants::PlanetConstants()
		: m_PlanetBuffer(ConstantBuffer::Create(sizeof GPUPlanetData, k_PlanetConstantBufferSlot))
		, m_GroundSampler(SamplerState::Create())
	{
		//<--- Ground textures tile, so they wrap; trilinear because a planet is all mip transitions ---<<
		m_GroundSampler->SetWrapMode(SamplerState::WrapMode::Repeat,
		                             SamplerState::WrapMode::Repeat,
		                             SamplerState::WrapMode::Repeat);

		m_GroundSampler->SetFilter(SamplerState::Filter::Linear,
		                           SamplerState::Filter::Linear,
		                           SamplerState::Filter::Linear);
	}

	void PlanetConstants::Bind(PlanetComponent& planet, const Matrix& planetToWorld)
	{
		LEV_PROFILE_FUNCTION();

		m_Data.PlanetToWorld = planetToWorld;
		m_Data.WorldToPlanet = planetToWorld.Invert();

		m_Data.SurfaceRadius = planet.Shape.Radius;
		m_Data.ElevationRange = Math::Max(planet.Shape.GetElevationRange(), Math::FloatEpsilon);

		m_Data.DetailStrength = planet.Detail.Strength;
		m_Data.DetailFrequency = planet.Detail.Frequency;
		m_Data.DetailSeed = planet.Shape.Seed;
		m_Data.TriplanarSharpness = Math::Max(planet.Detail.TriplanarSharpness, 1.0f);
		m_Data.DetailFadeStart = planet.Detail.FadeStart;
		m_Data.DetailFadeEnd = Math::Max(planet.Detail.FadeEnd, planet.Detail.FadeStart + 1.0f);

		m_Data.Time = Time::GetTimeSinceStartup().GetSeconds();

		m_Data.OceanShallowColor = Vector4(planet.Ocean.ShallowColor);
		m_Data.OceanDeepColor = Vector4(planet.Ocean.DeepColor);
		m_Data.OceanDepthFalloff = Math::Max(planet.Ocean.DepthFalloff, 1.0f);
		m_Data.OceanRoughness = Math::Clamp(planet.Ocean.Roughness, 0.01f, 1.0f);
		m_Data.OceanOpacity = Math::Saturate(planet.Ocean.Opacity);
		m_Data.OceanFresnel = planet.Ocean.FresnelStrength;
		m_Data.WaveStrength = planet.Ocean.WaveStrength;
		m_Data.WaveScale = planet.Ocean.WaveScale;
		m_Data.WaveSpeed = planet.Ocean.WaveSpeed;

		const Vector<PlanetBiome>& biomes = planet.GetBiomes();
		const auto biomeCount = static_cast<int32_t>(Math::Min<size_t>(biomes.size(), k_MaxPlanetBiomes));

		m_Data.BiomeCount = biomeCount;

		for (int32_t index = 0; index < biomeCount; ++index)
		{
			const PlanetBiome& biome = biomes[index];
			GPUPlanetBiome& target = m_Data.Biomes[index];

			target.ClimateRange = Vector4(biome.MinTemperature, biome.MaxTemperature,
			                              biome.MinHumidity, biome.MaxHumidity);

			target.Blends = Vector4(biome.TemperatureBlend, biome.HumidityBlend,
			                        biome.HeightBlend, biome.SlopeBlend);

			target.HeightSlope = Vector4(biome.MinHeight, biome.MaxHeight, biome.MinSlope, biome.MaxSlope);

			target.Tint = Vector4(biome.Tint.r, biome.Tint.g, biome.Tint.b, biome.Priority);

			target.Surface = Vector4(biome.Roughness, biome.Metallic, biome.TextureScale,
			                         static_cast<float>(biome.TextureIndex));

			target.Detail = Vector4(biome.NormalStrength, 0.0f, 0.0f, 0.0f);
		}

		BindTextures(planet);

		m_PlanetBuffer->SetData(&m_Data);
		m_PlanetBuffer->Bind(ShaderType::Vertex);
		m_PlanetBuffer->Bind(ShaderType::Pixel);
	}

	void PlanetConstants::BindTextures(PlanetComponent& planet)
	{
		m_AlbedoArray.reset();
		m_NormalArray.reset();
		m_RoughnessArray.reset();

		m_Data.TextureFlags = 0;

		if (planet.BiomeSet)
		{
			m_AlbedoArray = planet.BiomeSet->GetAlbedoArray();
			m_NormalArray = planet.BiomeSet->GetNormalArray();
			m_RoughnessArray = planet.BiomeSet->GetRoughnessArray();
		}

		// The flags are what the shader branches on. A set with no albedo array is not an error: the
		// biome tints alone already show continents, deserts and ice, which is what a planet looks
		// like before anybody has painted a texture for it.
		if (m_AlbedoArray)
		{
			m_AlbedoArray->Bind(k_AlbedoSlot, ShaderType::Pixel);
			m_Data.TextureFlags |= k_HasAlbedo;
		}

		if (m_NormalArray)
		{
			m_NormalArray->Bind(k_NormalSlot, ShaderType::Pixel);
			m_Data.TextureFlags |= k_HasNormal;
		}

		if (m_RoughnessArray)
		{
			m_RoughnessArray->Bind(k_RoughnessSlot, ShaderType::Pixel);
			m_Data.TextureFlags |= k_HasRoughness;
		}

		if (m_Data.TextureFlags != 0)
			m_GroundSampler->Bind(k_GroundSamplerSlot, ShaderType::Pixel);
	}

	void PlanetConstants::Unbind()
	{
		m_PlanetBuffer->Unbind(ShaderType::Vertex);
		m_PlanetBuffer->Unbind(ShaderType::Pixel);

		// Leaving the arrays bound would keep them at t0..t2 for the deferred lighting pass, which
		// wants the G-buffer there. See the note on the aliasing in Registers.hlsli.
		if (m_AlbedoArray)
			m_AlbedoArray->Unbind(k_AlbedoSlot, ShaderType::Pixel);

		if (m_NormalArray)
			m_NormalArray->Unbind(k_NormalSlot, ShaderType::Pixel);

		if (m_RoughnessArray)
			m_RoughnessArray->Unbind(k_RoughnessSlot, ShaderType::Pixel);

		if (m_Data.TextureFlags != 0)
			m_GroundSampler->Unbind(k_GroundSamplerSlot, ShaderType::Pixel);
	}
}
