#pragma once
#include "Math/Math.h"

namespace LevEngine
{
class SceneCamera
{
public:
	enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	SceneCamera();
	SceneCamera(float fov, float aspectRatio, float nearClip, float farClip);
	virtual ~SceneCamera() = default;
	void SetViewportSize(uint32_t width, uint32_t height);

	[[nodiscard]] const Matrix& GetProjection() const { return m_ProjectionType == ProjectionType::Orthographic ? m_OrthographicProjection : m_PerspectiveProjection; }

	void SetProjectionType(const ProjectionType type) { m_ProjectionType = type; }
	[[nodiscard]] ProjectionType GetProjectionType() const { return m_ProjectionType; }

	[[nodiscard]] Matrix GetPerspectiveProjection() const { return m_PerspectiveProjection; }
	float GetPerspectiveProjectionSliceDistance(float cascadeDistance) const;

protected:

	ProjectionType m_ProjectionType = ProjectionType::Perspective;
	float m_AspectRatio = 1.0f;
	uint32_t m_ViewportHeight = 0;

private:
	void RecalculateProjection();

	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// -- Orthographic -------------------------------------------------------------------------------------------
	// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
			
public:
	void SetOrthographic(float size, float nearClip, float farClip);

	void SetOrthographicSize(const float size) { m_OrthographicSize = size; RecalculateProjection(); }
	[[nodiscard]] float GetOrthographicSize() const { return m_OrthographicSize; }

	void SetOrthographicNear(const float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
	[[nodiscard]] float GetOrthographicNear() const { return m_OrthographicNear; }

	void SetOrthographicFar(const float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }
	[[nodiscard]] float GetOrthographicFar() const { return m_OrthographicFar; }
protected:
	float m_OrthographicSize = 10.0f;
	float m_OrthographicNear = -1.0f;
	float m_OrthographicFar = 1.0f;

	Matrix m_OrthographicProjection = Matrix::Identity;

	// ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	// -- Perspective --------------------------------------------------------------------------------------------
	// \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
			
public:
	void SetPerspective(float fov, float nearClip, float farClip);

	void SetFieldOfView(const float fov) { m_FieldOfView = DirectX::XMConvertToRadians(fov); RecalculateProjection(); }
	[[nodiscard]] float GetFieldOfView() const { return DirectX::XMConvertToDegrees(m_FieldOfView); }

	void SetPerspectiveNear(const float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
	[[nodiscard]] float GetPerspectiveNear() const { return m_PerspectiveNear; }

	void SetPerspectiveFar(const float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }
	[[nodiscard]] float GetPerspectiveFar() const { return m_PerspectiveFar; }

	[[nodiscard]] std::vector<Matrix> GetSplitPerspectiveProjections(const float* distances, int count) const;

protected:
	float m_FieldOfView = DirectX::XMConvertToRadians(45.0f);
	float m_PerspectiveNear = 0.1f;
	float m_PerspectiveFar = 1000.0f;

	Matrix m_PerspectiveProjection = Matrix::Identity;
};
}