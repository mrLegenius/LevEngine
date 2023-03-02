#pragma once
#include <SimpleMath.h>

class Camera
{
public:
	Camera() = default;

	explicit Camera(const DirectX::SimpleMath::Matrix& projection)
		: m_Projection(projection) { }

	virtual ~Camera() = default;

	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetProjection() const { return m_Projection; }
protected:
	DirectX::SimpleMath::Matrix m_Projection = DirectX::SimpleMath::Matrix::Identity;
};
