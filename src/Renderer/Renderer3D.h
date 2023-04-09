#pragma once
#include <Kernel/PointerUtils.h>

#include "D3D11ShadowMap.h"
#include "D3D11CascadeShadowMap.h"
#include "D3D11GBuffer.h"
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
	Vector4 PositionViewSpace;
	alignas(16) Vector3 Position;
	alignas(16) Vector3 Color{};
	float Range = 1.0f;
	float Smoothness = 0.75f;
	float Intensity = 1.0f;
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
	int HasEmissiveTexture = 0;
	int HasAmbientTexture = 0;
	int HasDiffuseTexture = 0;
	int HasSpecularTexture = 0;
	int HasNormalTexture = 0;
};

class Renderer3D
{
public:
	static void Init();
	static void Shutdown();


	static void BeginShadowPass(SceneCamera& camera, const std::vector<Matrix> mainCameraProjections, const Matrix& mainCameraView);
	static void EndShadowPass();
	static void DrawMeshShadow(const Matrix& model, const MeshRendererComponent& meshRenderer);


	static void BeginScene(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndScene();
	static void DrawOpaqueMesh(const Matrix& model, const MeshRendererComponent& meshRenderer);

	static void BeginDeferred(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndDeferred();
	static void BeginDeferredDirLightningSubPass(const SceneCamera& camera);
	static void BeginDeferredPositionalLightningSubPass1(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void BeginDeferredPositionalLightningSubPass2(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndDeferredLightningPass();

	static void DrawDeferredMesh(const Matrix& model, const MeshRendererComponent& meshRenderer);

	static void DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer);
	static void DrawSkybox(const SkyboxRendererComponent& renderer);

	static void SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight);
	static void AddPointLights(const Vector4& positionViewSpace, const Vector3& position, const PointLightComponent& pointLight);
	static void UpdateLights();
	static void RenderSphere(const Matrix& model);

private:
	static Ref<D3D11ConstantBuffer> m_ModelConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_CameraConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_LightningConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_ShadowMapConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_MaterialConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_ScreenToViewParamsConstantBuffer;

	static Ref<D3D11ShadowMap> m_ShadowMap;
	static Ref<D3D11CascadeShadowMap> m_CascadeShadowMap;

	static Ref<D3D11GBuffer> m_GBuffer;

	static Matrix m_PerspectiveViewProjection;
	static Matrix m_ViewProjection;

	static Ref<SkyboxMesh> s_SkyboxMesh;

	static PointLightData s_PointLights[RenderSettings::MaxPointLights];
	static LightningData s_LightningData;
	static ShadowData s_ShadowData;
};
