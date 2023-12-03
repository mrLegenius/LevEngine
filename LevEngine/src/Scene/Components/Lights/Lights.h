#pragma once
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(DirectionalLightComponent);
	struct DirectionalLightComponent
	{
		Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		DirectionalLightComponent();
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	REGISTER_PARSE_TYPE(PointLightComponent);
	struct PointLightComponent
	{
		Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		float Range = 10.0f;
		float Smoothness = 0.75f;
		float Intensity = 1.0f;

		PointLightComponent();
		PointLightComponent(const PointLightComponent&) = default;
	};

	REGISTER_PARSE_TYPE(SpotLightComponent);
	struct SpotLightComponent
	{
		Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		float Range = 10.0f;
		float Angle = 45.0f;
		float Smoothness = 0.75f;
		float Intensity = 1.0f;

		SpotLightComponent();
		SpotLightComponent(const SpotLightComponent&) = default;
	};
}
