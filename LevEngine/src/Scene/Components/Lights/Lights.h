#pragma once

#include "Kernel/Core.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(DirectionalLightComponent);
	struct LEV_API DirectionalLightComponent
	{
		Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		DirectionalLightComponent();
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	REGISTER_PARSE_TYPE(PointLightComponent);
	struct LEV_API PointLightComponent
	{
		Color color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		float Range = 10.0f;
		float Smoothness = 0.75f;
		float Intensity = 1.0f;

		PointLightComponent();
		PointLightComponent(const PointLightComponent&) = default;
	};

	REGISTER_PARSE_TYPE(SpotLightComponent);
	struct LEV_API SpotLightComponent
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
