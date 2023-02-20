#pragma once
#include <SimpleMath.h>

struct Transform
{
	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 rotation = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;

	DirectX::SimpleMath::Matrix GetModel() const
	{
		return DirectX::SimpleMath::Matrix::CreateScale(scale) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(
				DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotation)) *
			DirectX::SimpleMath::Matrix::CreateTranslation(position);
	}
};
