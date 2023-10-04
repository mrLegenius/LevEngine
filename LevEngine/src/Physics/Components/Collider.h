#pragma once
#include "Math/Math.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(BoxCollider);
	REGISTER_PARSE_TYPE(SphereCollider);
	
	struct BoxCollider final 
	{
		Vector3 extents;
		Vector3 offset;

		BoxCollider();
	};

	struct SphereCollider final
	{
		float radius = 1;
		Vector3 offset;

		SphereCollider();
	};
}
