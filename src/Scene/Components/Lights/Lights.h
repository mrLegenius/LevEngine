#pragma once
#include "Kernel/Color.h"

namespace LevEngine
{
	struct DirectionalLightComponent
	{
		Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	struct PointLightComponent
	{
		Color color = LevEngine::Color(1.0f, 1.0f, 1.0f, 1.0f);

		float Range = 10.0f;
		float Smoothness = 0.75f;
		float Intensity = 1.0f;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};
}
