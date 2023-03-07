#pragma once
#include <SimpleMath.h>

struct Transform
{
	Transform()
	{
		RecalculateModel();
	}


	[[nodiscard]] const DirectX::SimpleMath::Matrix& GetModel() const { return model; }

	void SetScale(const DirectX::SimpleMath::Vector3 value)
	{
		if (scale == value) return;

		scale = value;
		RecalculateModel();
	}

	const DirectX::SimpleMath::Vector3& GetPosition() const { return position; }
	const DirectX::SimpleMath::Vector3& GetScale() const { return scale; }
	const DirectX::SimpleMath::Vector3& GetRotation() const { return rotation; }
	const DirectX::SimpleMath::Vector3& GetRotationDegrees() const
	{
		auto rot = rotation;
		rot.x = DirectX::XMConvertToDegrees(rot.x);
		rot.y = DirectX::XMConvertToDegrees(rot.y);
		rot.z = DirectX::XMConvertToDegrees(rot.z);
		return rot;
	}

	void SetPosition(const DirectX::SimpleMath::Vector3 value)
	{
		if (position == value) return;

		position = value;
		RecalculateModel();
	}

	void SetRotation(const DirectX::SimpleMath::Vector3 value)
	{
		auto newRotation = value;
		newRotation.x = DirectX::XMConvertToRadians(newRotation.x);
		newRotation.y = DirectX::XMConvertToRadians(newRotation.y);
		newRotation.z = DirectX::XMConvertToRadians(newRotation.z);

		if (rotation == newRotation) return;

		rotation = newRotation;
		RecalculateModel();
	}

	void MoveForward(const float value)
	{
		const auto dir = GetForwardDirection();
		SetPosition(position + dir * value);
	}

	void MoveBackward(const float value)
	{
		const auto dir = GetForwardDirection();
		SetPosition(position - dir * value);
	}

	void MoveLeft(const float value)
	{
		const auto dir = GetRightDirection();
		SetPosition(position - dir * value);
	}

	void MoveRight(const float value)
	{
		const auto dir = GetRightDirection();
		SetPosition(position + dir * value);
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

	void SetPositionX(const float x)
	{
		if ( abs(x - position.x) < 0.00001f) return;

		position.x = x;
		RecalculateModel();
	}

	void SetPositionY(const float y)
	{
		if (abs(y - position.y) < 0.00001f) return;

		position.y = y;
		RecalculateModel();
	}

	void SetPositionZ(const float z)
	{
		if (abs(z - position.z) < 0.00001f) return;

		position.z = z;
		RecalculateModel();
	}

private:

	DirectX::SimpleMath::Matrix model;

	DirectX::SimpleMath::Vector3 position = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 rotation = DirectX::SimpleMath::Vector3::Zero;
	DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3::One;

	void RecalculateModel()
	{
		model = DirectX::SimpleMath::Matrix::CreateScale(scale) *
			DirectX::SimpleMath::Matrix::CreateFromQuaternion(
				DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(rotation)) *
			DirectX::SimpleMath::Matrix::CreateTranslation(position);
	}
};
