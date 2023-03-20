#pragma once
#include <Kernel/PointerUtils.h>

#include "D3D11DepthBuffer.h"
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
	alignas(16) Vector3 Ambient{};
	alignas(16) Vector3 Diffuse{};
	alignas(16) Vector3 Specular{};
};

struct PointLightData
{
	alignas(16) Vector3 Position;

	float Constant = 1.0f;
	float Linear = 0.09f;
	float Quadratic = 0.032f;
	float _pad = 0;

	alignas(16) Vector3 Ambient{};
	alignas(16) Vector3 Diffuse{};
	alignas(16) Vector3 Specular{};
};

class Renderer3D
{
public:
	static void Init();
	static void Shutdown();
	static void BeginShadowPass(SceneCamera& camera);
	static void EndShadowPass();

	static void BeginScene(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void EndScene();
	static void DrawMeshShadow(const Matrix& model, const MeshRendererComponent& meshRenderer);

	static void DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer);
	static void DrawSkybox(const SkyboxRendererComponent& renderer);

	static void SetDirLight(const Vector3& dirLightDirection, const Matrix& dirLightViewMatrix, const DirectionalLightComponent& dirLight);
	static void AddPointLights(const Vector3& position, const PointLightComponent& pointLight);
	static void UpdateLights();

private:
	static Ref<D3D11ConstantBuffer> m_ModelConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_CameraConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_DirLightConstantBuffer;
	static Ref<D3D11ConstantBuffer> m_ShadowMapConstantBuffer;

	static Ref<D3D11DepthBuffer> m_ShadowMapBuffer;

	static Matrix m_PerspectiveViewProjection;
	static Matrix m_ViewProjection;

	static Ref<SkyboxMesh> s_SkyboxMesh;

	static std::vector<PointLightData> s_PointLights;
	static DirLightData s_DirLight;
};
