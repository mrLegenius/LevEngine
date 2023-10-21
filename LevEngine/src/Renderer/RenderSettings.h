#pragma once
#include "Math/Vector3.h"

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
	static constexpr RenderTechniqueType RenderTechnique = RenderTechniqueType::Deferred;
	static constexpr RendererAPI RendererAPI = RendererAPI::D3D11;
	
	static constexpr bool EnableShaderDebug = false;
	static constexpr bool UseFrustumCulling = true;

	static constexpr uint32_t ShadowMapSlot = 9;
	static constexpr uint32_t MaterialSlot = 4;

	static constexpr uint32_t MaxParticles = 1024 * 8;
	
	//Shadows
	static constexpr int CascadeCount = 4;
	static constexpr float CascadeDistances[CascadeCount] = { 0.1f, 0.3f, 0.5f, 1.0f };
	static constexpr float ShadowMapResolution = 2048;

	//Lightning
	static constexpr int MaxPointLights = 200;
	static constexpr Vector3 GlobalAmbient{ 0.03f, 0.03f, 0.03f };
	
	//Post-processing
	static constexpr uint32_t LuminanceMapSize = 1024;

	static constexpr float BloomThreshold = 0.2f;
	static constexpr float BloomMagnitude = 1.0f;
	static constexpr float BloomBlurSigma = 0.8f;
	static constexpr float AdaptationRate = 0.5f;
	static constexpr float KeyValue = 0.01f;
	static constexpr float MinExposure = 0.5f;
	static constexpr float MaxExposure = 3.00f;
};

struct RenderDebugSettings
{
	static constexpr bool DrawBoundingVolumes = false;
};
}
