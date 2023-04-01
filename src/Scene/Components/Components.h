#pragma once
#include <string>

#include "OrbitCamera.h"
#include "Renderer/Camera/SceneCamera.h"
#include "Renderer/D3D11Texture.h"
#include "Renderer/3D/Mesh.h"
#include "Renderer/D3d11Shader.h"
#include "Kernel/UUID.h"
#include "Physics/Components/Collider.h"
#include "Physics/Components/Rigidbody.h"
#include "Renderer/D3D11TextureCube.h"
#include "Renderer/Material.h"

struct IDComponent
{
	LevEngine::UUID ID;

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
	Ref<D3D11Shader> shader;
	Ref<Texture> emissiveTexture;
	Ref<Texture> ambientTexture;
	Ref<Texture> diffuseTexture;
	Ref<Texture> specularTexture;
	Ref<Texture> normalTexture;
	Material material;
	bool castShadow = true;

	MeshRendererComponent() = default;
	MeshRendererComponent(const Ref<D3D11Shader>& shader,
		const Ref<Mesh>& mesh) : mesh(mesh), shader(shader)
	{ }
	MeshRendererComponent(const MeshRendererComponent&) = default;
};

struct SkyboxRendererComponent
{
	Ref<D3D11TextureCube> texture;

	SkyboxRendererComponent(const Ref<D3D11TextureCube> texture) : texture(texture) { }
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
