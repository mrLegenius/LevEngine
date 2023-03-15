#include "SceneCamera.h"

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

void SceneCamera::SetViewportSize(const uint32_t width, const uint32_t height)
{
	m_AspectRatio = height == 0 ? 0 : static_cast<float>(width) / static_cast<float>(height);

	RecalculateProjection();
}

void SceneCamera::RecalculateProjection()
{
	const float horizontal = m_OrthographicSize * 800;
	const float vertical = m_AspectRatio * horizontal;

	m_OrthographicProjection = Matrix::CreateOrthographic(vertical, horizontal, m_OrthographicNear, m_OrthographicFar);

	m_PerspectiveProjection = Matrix::CreatePerspectiveFieldOfView(m_FieldOfView, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
}