#pragma once
#include "Math/Vector3.h"
#include "Scene/Serializers/SerializerUtils.h"

namespace LevEngine
{
enum class RendererAPI
{
	None = 0,
	D3D11 = 5,
	OpenGL = 10,
};

enum class RenderTechniqueType
{
	Forward = 1,
	Deferred = 2,
	ForwardPlus = 3,
};

struct RenderSettings
{
	//General
	inline static RenderTechniqueType RenderTechnique = RenderTechniqueType::Deferred;
	inline static RendererAPI RendererAPI = RendererAPI::D3D11;

	inline static bool UseFrustumCulling = true;
	
	static constexpr bool EnableShaderDebug = false;
	static constexpr uint32_t ShadowMapSlot = 9;
	static constexpr uint32_t MaterialSlot = 4;

	inline static uint32_t MaxParticles = 1024 * 8;
	
	//Shadows
	static constexpr int CascadeCount = 4;
	static constexpr float CascadeDistances[CascadeCount] = { 0.1f, 0.3f, 0.5f, 1.0f };
	static constexpr float ShadowMapResolution = 2048;

	//Lightning
	static constexpr int MaxPointLights = 200;
	static constexpr Vector3 GlobalAmbient{ 0.03f, 0.03f, 0.03f };
	
	//Post-processing
	static constexpr uint32_t LuminanceMapSize = 1024;

	inline static float BloomThreshold = 3.0f;
	inline static float BloomMagnitude = 1.0f;
	inline static float BloomBlurSigma = 0.8f;
	inline static float AdaptationRate = 0.5f;
	inline static float KeyValue = 0.2f;
	inline static float MinExposure = 0.5f;
	inline static float MaxExposure = 3.00f;
};

struct RenderDebugSettings
{
	static constexpr bool DrawBoundingVolumes = false;
};
}
