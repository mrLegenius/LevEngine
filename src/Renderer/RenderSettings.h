#pragma once
#include <SimpleMath.h>

struct RenderSettings
{
	//Shadows
	static constexpr int CascadeCount = 4;
	static constexpr float CascadeDistances[CascadeCount] = { 0.1f, 0.3f, 0.5f, 1.0f };
	//static constexpr float CascadeDistances[CascadeCount] = { 1.0f };
	static constexpr float ShadowMapResolution = 2048;

	//Lightning
	static constexpr int MaxPointLights = 100;
	static constexpr DirectX::SimpleMath::Vector3 GlobalAmbient{ 0.99f, 0.99f, 0.99f };
	static constexpr bool DeferredRendering = true;
};

