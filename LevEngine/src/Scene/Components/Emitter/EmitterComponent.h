#pragma once
#include "Assets/TextureAsset.h"
#include "Renderer/Texture.h"
namespace LevEngine
{
	struct EmitterComponent
	{
		EmitterComponent();

		struct BirthParams
		{
			bool RandomVelocity = false;
			Vector3 Velocity = Vector3::Zero;
			Vector3 VelocityB = Vector3::Zero;

			bool RandomStartPosition = false;
			Vector3 Position = Vector3::Zero;
			Vector3 PositionB = Vector3::Zero;

			bool RandomStartColor = false;
			Color StartColor = Color{ 1, 1, 1, 1 };
			Color StartColorB = Color{ 1, 1, 1, 1 };
			Color EndColor = Color{ 1, 1, 1, 1 };

			bool RandomStartSize = false;
			float StartSize = 1.0f;
			float StartSizeB = 1.0f;
			float EndSize = 1.0f;

			bool RandomStartLifeTime = false;
			float LifeTime = 1;
			float LifeTimeB = 1;
			float GravityScale = 0;
		};

		uint32_t MaxParticles = 10000;
		float Rate = 1.0f;
		BirthParams Birth;
		Ref<TextureAsset> Texture;

	private:
		float Timer = 0.0f;
		friend class ParticlePass;
	};
}
