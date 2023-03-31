#pragma once
#include <Kernel/PointerUtils.h>

#include "D3D11ShadowMap.h"
#include "D3D11CascadeShadowMap.h"
#include "RenderSettings.h"
#include "3D/Mesh.h"
#include "3D/SkyboxMesh.h"
#include "Scene/Components/Components.h"

struct MeshModelBufferData
{
	Matrix Model;
};

struct DirLightData
{
	alignas(16) Vector3 Direction{};
	alignas(16) Vector3 Color {};
};

struct PointLightData
{
	alignas(16) Vector3 Position;
	alignas(16) Vector3 Attenuation = Vector3(1.0f, 0.09f, 0.032f);
	alignas(16) Vector3 Color{};
};

struct LightningData
{
	DirLightData DirLight;
	PointLightData PointLightsData[RenderSettings::MaxPointLights];
	alignas(16) Vector3 GlobalAmbient;
	uint32_t PointLightsCount = 0;
};

struct ShadowData
{
	alignas(16) Matrix ViewProjection[RenderSettings::CascadeCount];
	alignas(16) float Distances[RenderSettings::CascadeCount];
	alignas(16) float ShadowMapDimensions;
};

struct MaterialData
{
	alignas(16) Vector3 Emissive = Vector3{ 0, 0, 0 };
	alignas(16) Vector3 Ambient = Vector3{ 0, 0, 0 };
	alignas(16) Vector3 Diffuse = Vector3{ 0, 0, 0 };
	alignas(16) Vector3 Specular = Vector3{ 0, 0, 0 };

	float Shininess = 2;
	int UseTexture;
};

class Renderer3D
{
public:
	static void Init();
	static void Shutdown();
	static void BeginShadowPass(SceneCamera& camera, const std::vector<Matrix> mainCameraProjections, const Matrix& mainCameraView);
	static void EndShadowPass();

	static void BeginScene(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndScene();
	static void DrawMeshShadow(const Matrix& model, const MeshRendererComponent& meshRenderer);

	static void DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer);
	static void DrawSkybox(const SkyboxRendererComponent& renderer);

	static void SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight);
	static void AddPointLights(const Vector3& position, const PointLightComponent& pointLight);
	static void UpdateLights();

private:
	static Ref<D3D11ConstantBuffer> m_ModelConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_CameraConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_LightningConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_ShadowMapConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_MaterialConstantBuffer;

	static Ref<D3D11ShadowMap> m_ShadowMap;
	static Ref<D3D11CascadeShadowMap> m_CascadeShadowMap;

	static Matrix m_PerspectiveViewProjection;
	static Matrix m_ViewProjection;

	static Ref<SkyboxMesh> s_SkyboxMesh;

	static PointLightData s_PointLights[RenderSettings::MaxPointLights];
	static LightningData s_LightningData;
	static ShadowData s_ShadowData;
};
