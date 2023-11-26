#pragma once
#include "Math/Math.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(LegacyBoxCollider);
	REGISTER_PARSE_TYPE(LegacySphereCollider);
	
	struct LegacyBoxCollider final 
	{
		Vector3 extents;
		Vector3 offset;

		LegacyBoxCollider();
	};

	struct LegacySphereCollider final
	{
		float radius = 1;
		Vector3 offset;

		LegacySphereCollider();
	};
}
