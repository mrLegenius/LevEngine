#pragma once
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

struct RenderSettings
{
	//Shadows
	static constexpr int CascadeCount = 4;
	static constexpr float CascadeDistances[CascadeCount] = { 0.1f, 0.3f, 0.5f, 1.0f };
	//static constexpr float CascadeDistances[CascadeCount] = { 1.0f };
	static constexpr float ShadowMapResolution = 2048;

	//Lightning
	static constexpr int MaxPointLights = 100;
	static constexpr Vector3 GlobalAmbient{ 0.3f, 0.3f, 0.3f };
	static constexpr bool DeferredRendering = true;
};

