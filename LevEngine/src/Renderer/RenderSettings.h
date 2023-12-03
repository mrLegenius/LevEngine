#pragma once

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
	static RenderTechniqueType RenderTechnique;
	static RendererAPI RendererAPI;

	static bool UseFrustumCulling;
	
	static constexpr bool EnableShaderDebug = false;
	static constexpr uint32_t ShadowMapSlot = 9;
	static constexpr uint32_t MaterialSlot = 4;

	static uint32_t MaxParticles;
	
	//Shadows
	static constexpr int CascadeCount = 4;
	static constexpr float CascadeDistances[CascadeCount] = { 0.1f, 0.3f, 0.5f, 1.0f };
	static constexpr float ShadowMapResolution = 2048;

	//Lighting
	static constexpr int MaxLights = 100;
	
	//Post-processing
	static constexpr uint32_t LuminanceMapSize = 1024;

	inline static bool IsBloomEnabled = true;
	inline static float BloomThreshold = 3.0f;
	inline static float BloomMagnitude = 1.0f;
	inline static float BloomBlurSigma = 0.8f;

	inline static bool IsEyeAdaptationEnabled = true;
	inline static float AdaptationRate = 0.5f;
	inline static float KeyValue = 0.2f;
	
	inline static float MinExposure = 0.5f;
	inline static float MaxExposure = 3.00f;
	inline static float ManualExposure = 1.00f;
	
	inline static bool IsVignetteEnabled = true;
	inline static float VignetteRadius = 0.5f;
	inline static float VignetteSoftness = 0.5f;
	inline static float VignetteIntensity = 0.7f;
	inline static Vector2 VignetteCenter = {0.5f, 0.5f};
	inline static Color VignetteColor;
};

struct RenderDebugSettings
{
	static constexpr bool DrawBoundingVolumes = false;
};
}
