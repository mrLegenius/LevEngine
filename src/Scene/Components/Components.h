#pragma once
#include <string>

#include "OrbitCamera.h"
#include "Kernel/Color.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Renderer/Texture.h"
#include "Renderer/3D/Mesh.h"
#include "Renderer/Shader.h"
#include "Kernel/UUID.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/Rigidbody.h"
#include "Renderer/Material.h"
namespace LevEngine
{
	struct IDComponent
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		explicit TagComponent(std::string other)
			: tag(std::move(other)) { }
	};

	struct MeshRendererComponent
	{
		Ref<Mesh> mesh;
		Ref<Shader> shader;
		Ref<Texture> emissiveTexture;
		Ref<Texture> ambientTexture;
		Ref<Texture> diffuseTexture;
		Ref<Texture> specularTexture;
		Ref<Texture> normalTexture;
		Material material;
		bool castShadow = true;

		MeshRendererComponent() = default;
		MeshRendererComponent(const Ref<Shader>& shader,
			const Ref<Mesh>& mesh) : mesh(mesh), shader(shader)
		{ }
		MeshRendererComponent(const MeshRendererComponent&) = default;
	};

	struct SkyboxRendererComponent
	{
		Ref<Texture> texture;

		SkyboxRendererComponent(const Ref<Texture> texture) : texture(texture) { }
		SkyboxRendererComponent() = default;
		SkyboxRendererComponent(const SkyboxRendererComponent&) = default;
	};

	struct DirectionalLightComponent
	{
		Vector3 Color = Vector3(1.0f);

		DirectionalLightComponent() = default;
		DirectionalLightComponent(const DirectionalLightComponent&) = default;
	};

	struct PointLightComponent
	{
		Vector3 Color = Vector3(1.0f);

		float Range = 10.0f;
		float Smoothness = 0.75f;
		float Intensity = 1.0f;

		PointLightComponent() = default;
		PointLightComponent(const PointLightComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera camera;
		bool isMain = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

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

	template<typename... Component>
	struct ComponentGroup { };

	using AllComponents = ComponentGroup<
		Transform,
		Rigidbody,
		BoxCollider,
		SphereCollider,
		MeshRendererComponent,
		SkyboxRendererComponent,
		OrbitCamera,
		CameraComponent,
		DirectionalLightComponent,
		PointLightComponent>;
}
