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

	void MoveForward(const float value)
	{
		const auto forward = GetForwardDirection();
		position += forward * value;
	}

	void MoveBackward(const float value)
	{
		const auto forward = GetForwardDirection();
		position -= forward * value;
	}

	void MoveLeft(const float value)
	{
		const auto forward = GetRightDirection();
		position -= forward * value;
	}

	void MoveRight(const float value)
	{
		const auto forward = GetRightDirection();
		position += forward * value;
	}

	DirectX::SimpleMath::Vector3 GetUpDirection() const
	{
		DirectX::SimpleMath::Vector3 dir = XMVector3Rotate(DirectX::SimpleMath::Vector3::Up, GetOrientation());
		dir.Normalize();
		return dir;
	}

	DirectX::SimpleMath::Vector3 GetRightDirection() const
	{
		DirectX::SimpleMath::Vector3 dir = XMVector3Rotate(DirectX::SimpleMath::Vector3::Right, GetOrientation());
		dir.Normalize();
		return dir;
	}

	DirectX::SimpleMath::Vector3 GetForwardDirection() const
	{
		DirectX::SimpleMath::Vector3 dir = XMVector3Rotate(DirectX::SimpleMath::Vector3::Forward, GetOrientation());
		dir.Normalize();
		return dir;
	}

	DirectX::SimpleMath::Quaternion GetOrientation() const
	{
		return DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotation);
	}
};
