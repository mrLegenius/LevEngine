#pragma once
#include <Kernel/PointerUtils.h>

#include "D3D11ShadowMap.h"
#include "D3D11CascadeShadowMap.h"
#include "D3D11DeferredTechnique.h"
#include "D3D11ForwardTechnique.h"
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

	static void BeginScene(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndScene();
	static void DrawOpaqueMesh(const Matrix& model, const MeshRendererComponent& meshRenderer);

	static void BeginDeferred(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndDeferred();
	static void BeginDeferredDirLightningSubPass(const SceneCamera& camera);
	static void BeginDeferredPositionalLightningSubPass(const SceneCamera& camera, const Matrix& viewMatrix,
	                                                    const Vector3& position);
	static void BeginDeferredPositionalLightningSubPass1(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void BeginDeferredPositionalLightningSubPass2(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndDeferredLightningPass();

	static void DrawDeferredSkybox(const SkyboxRendererComponent& renderer);
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
	static Ref<D3D11ConstantBuffer> m_MaterialConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_ScreenToViewParamsConstantBuffer;

	static Ref<D3D11DeferredTechnique> m_GBuffer;
	static Ref<D3D11ForwardTechnique> s_ForwardTechnique;

	static Matrix m_PerspectiveViewProjection;
	static Matrix m_ViewProjection;

	static Ref<SkyboxMesh> s_SkyboxMesh;

	static PointLightData s_PointLights[RenderSettings::MaxPointLights];
	static LightningData s_LightningData;
};
