#include "levpch.h"
#include "SceneCamera.h"

#include "Scene/Components/Transform/Transform.h"

namespace LevEngine
{
SceneCamera::SceneCamera()
{
	RecalculateProjection();
}

void SceneCamera::SetOrthographic(const float size, const float nearClip, const float farClip)
{
	m_OrthographicSize = size;
	m_OrthographicNear = nearClip;
	m_OrthographicFar = farClip;

	RecalculateProjection();
}

void SceneCamera::SetPerspective(const float fov, const float nearClip, const float farClip)
{
	m_FieldOfView = DirectX::XMConvertToRadians(fov);
	m_PerspectiveNear = nearClip;
	m_PerspectiveFar = farClip;

	RecalculateProjection();
}

Vector<Matrix> SceneCamera::GetSplitPerspectiveProjections(const float* distances, const int count) const
{
	Vector<Matrix> projections;

	float min = m_PerspectiveNear;

	for (int i = 0; i < count; ++i)
	{
		const float max = Math::Lerp(m_PerspectiveNear, m_PerspectiveFar, distances[i]);
		auto projection = Matrix::CreatePerspectiveFieldOfView(m_FieldOfView, m_AspectRatio, min, max);

		projections.emplace_back(projection);

		min = max;
	}
	return projections;
}

float SceneCamera::GetPerspectiveProjectionSliceDistance(const float distance) const
{
	return m_PerspectiveNear + Math::Lerp(m_PerspectiveNear, m_PerspectiveFar, distance);
}

void SceneCamera::SetViewportSize(const uint32_t width, const uint32_t height)
{
	m_ViewportHeight = height;
	m_AspectRatio = height == 0 ? 0 : static_cast<float>(width) / static_cast<float>(height);

	RecalculateProjection();
}

void SceneCamera::RecalculateProjection()
{
	if (m_ProjectionType == ProjectionType::Orthographic && m_ViewportHeight != 0)
	{
		const float height = m_OrthographicSize * static_cast<float>(m_ViewportHeight);
		const float width = m_AspectRatio * height;

		m_OrthographicProjection = Matrix::CreateOrthographic(width, height, m_OrthographicNear, m_OrthographicFar);
	}

	m_PerspectiveProjection = Matrix::CreatePerspectiveFieldOfView(m_FieldOfView, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
}

void SceneCamera::RecalculateFrustum(const Transform& cameraTransform)
{
	//TODO: Make frustum for orthographic
	const float zFar = m_PerspectiveFar;
	const float zNear = m_PerspectiveNear;

	const Vector3 position = cameraTransform.GetWorldPosition();
	const Vector3 front = cameraTransform.GetForwardDirection();
	const Vector3 right = cameraTransform.GetRightDirection();
	const Vector3 up = cameraTransform.GetUpDirection();
	
	const float halfVSide = zFar * tanf(m_FieldOfView * .5f);
	const float halfHSide = halfVSide * m_AspectRatio;
	const Vector3 frontMultFar = zFar * front;
	
	m_Frustum.nearFace = Plane{ position + zNear * front, front };
	m_Frustum.farFace = { position + frontMultFar, -front };
	m_Frustum.rightFace = { position, (frontMultFar - right * halfHSide).Cross(up) };
	m_Frustum.leftFace = { position, up.Cross(frontMultFar + right * halfHSide) };
	m_Frustum.topFace = { position, right.Cross(frontMultFar - up * halfVSide)};
	m_Frustum.bottomFace = { position, (frontMultFar + up * halfVSide).Cross(right)};
}
}
