#pragma once
#include "Renderer/Texture.h"
namespace LevEngine
{
	struct EmitterComponent
	{
		struct BirthParams
		{
			bool RandomVelocity = false;
			Vector3 Velocity = Vector3::Zero;
			Vector3 VelocityB = Vector3::Zero;

			bool RandomStartPosition = false;
			Vector3 Position = Vector3::Zero;
			Vector3 PositionB = Vector3::Zero;

			bool RandomStartColor = false;
			Color StartColor;
			Color StartColorB;
			Color EndColor = Color{ 1, 1, 1, 1 };

			bool RandomStartSize = false;
			float StartSize = 0.1f;
			float StartSizeB = 0.1f;
			float EndSize = 0.1f;

			bool RandomStartLifeTime = false;
			float LifeTime = 1;
			float LifeTimeB = 1;
			float GravityScale = 0;
		};

		uint32_t MaxParticles = 10000;
		float Rate;
		BirthParams Birth;
		Ref<Texture> Texture;
	};
}
