#pragma once
#include "ConstantBuffer.h"
#include "RenderTechnique.h"
#include "RenderSettings.h"
#include "Scene/Components/Lights/Lights.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"

namespace LevEngine
{
struct MeshModelBufferData
{
	Matrix Model;
	Matrix TransposedInvertedModel;
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

class Renderer3D
{
public:
	static void Init();

	static void SetCameraBuffer(const SceneCamera& camera, const Matrix& viewMatrix, const Vector3& position);
	static void DrawMesh(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
	static void DrawMesh(const Matrix& model, const MeshRendererComponent& meshRenderer, const Ref<Shader>& shader);
	
	static void DrawLineList(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);
	static void DrawLineStrip(const Matrix& model, const Ref<Mesh>& mesh, const Ref<Shader>& shader);

	static void SetDirLight(const Vector3& dirLightDirection, const DirectionalLightComponent& dirLight);
	static void AddPointLights(const Vector4& positionViewSpace, const Vector3& position, const PointLightComponent& pointLight);
	static void UpdateLights();
	static void RenderSphere(const Matrix& model, const Ref<Shader>& shader);

private:
	static Ref<ConstantBuffer> m_ModelConstantBuffer;
	static Ref<ConstantBuffer> m_CameraConstantBuffer;
	static Ref<ConstantBuffer> m_LightningConstantBuffer;
	static Ref<ConstantBuffer> m_ScreenToViewParamsConstantBuffer;

	static Matrix m_ViewProjection;

	static PointLightData s_PointLights[RenderSettings::MaxPointLights];
	static LightningData s_LightningData;
};
}
