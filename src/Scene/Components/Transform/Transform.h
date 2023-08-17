#pragma once
#include <set>

#include "yaml-cpp/src/exp.h"

struct Transform
{
	Transform()
	{
		ForceRecalculateModel();
	}

	[[nodiscard]] const Matrix& GetModel() const { return model; }

	[[nodiscard]] Vector3 GetLocalPosition() const { return position; }
	[[nodiscard]] Vector3 GetLocalScale() const { return scale; }
	[[nodiscard]] Vector3 GetLocalRotation() const { return rotation; }

	[[nodiscard]] Vector3 GetWorldPosition() const
	{
		Vector3 localPos = position;
		if (parent)
		{
			localPos *= parent->GetWorldScale();
			localPos = Vector3::Transform(localPos, parent->GetWorldOrientation());
			localPos += parent->GetWorldPosition();
		}
		return localPos;
	}

	[[nodiscard]] Vector3 GetWorldScale() const
	{
		return parent ? parent->GetWorldScale() * scale : scale;
	}
	[[nodiscard]] Vector3 GetWorldRotation() const
	{
		return parent ? (GetLocalOrientation() * parent->GetWorldOrientation()).ToEuler() : rotation;
	}

	[[nodiscard]] Vector3 GetRotationDegrees() const
	{
		auto rot = rotation;
		rot.x = DirectX::XMConvertToDegrees(rot.x);
		rot.y = DirectX::XMConvertToDegrees(rot.y);
		rot.z = DirectX::XMConvertToDegrees(rot.z);
		return rot;
	}

	const std::set<Transform*>& GetChildren() const { return children; }
	[[nodiscard]] uint32_t GetChildrenCount() const { return children.size(); }

	void SetParent(Transform* value, bool keepWorldTransform = true);
	void SetLocalPosition(const Vector3 value) { position = value; }
	void SetWorldPosition(const Vector3 value)
	{
		position = value;

		if (parent)
		{
			position -= parent->GetWorldPosition();
			auto rot = parent->GetWorldOrientation();
			rot.Conjugate();
			position = Vector3::Transform(position, rot);
		}
	}

	void SetWorldRotation(const Vector3 value)
	{
		auto newRotation = value;
		newRotation.x = DirectX::XMConvertToRadians(newRotation.x);
		newRotation.y = DirectX::XMConvertToRadians(newRotation.y);
		newRotation.z = DirectX::XMConvertToRadians(newRotation.z);

		rotation = newRotation;

		if (parent)
			rotation -= parent->GetWorldRotation();
	}

	void SetWorldRotation(const Quaternion value)
	{
		rotation = value.ToEuler();

		if (parent)
			rotation -= parent->GetWorldRotation();
	}

	void SetLocalRotation(const Vector3 value)
	{
		auto newRotation = value;
		newRotation.x = DirectX::XMConvertToRadians(newRotation.x);
		newRotation.y = DirectX::XMConvertToRadians(newRotation.y);
		newRotation.z = DirectX::XMConvertToRadians(newRotation.z);

		rotation = newRotation;
	}

	void SetLocalRotationRadians(const Vector3 value)
	{
		rotation = value;
	}

	void SetLocalRotation(const Quaternion value)
	{
		rotation = value.ToEuler();
	}

	void SetLocalScale(const Vector3 value)
	{
		scale = value;
	}

	void SetWorldScale(const Vector3 value)
	{
		if (parent)
		{
			const auto parentScale = parent->GetWorldScale();
			scale.x = DirectX::XMScalarNearEqual(parentScale.x, 0, 0.0001f) ? 0 : value.x / parentScale.x;
			scale.y = DirectX::XMScalarNearEqual(parentScale.y, 0, 0.0001f) ? 0 : value.y / parentScale.y;
			scale.z = DirectX::XMScalarNearEqual(parentScale.z, 0, 0.0001f) ? 0 : value.z / parentScale.z;
		}
		else
		{
			scale = value;
		}
	}


	void Move(const Vector3 value) { position += value; }

	void MoveForward(const float value)
	{
		const auto dir = GetForwardDirection();
		SetLocalPosition(position + dir * value);
	}
	void MoveBackward(const float value)
	{
		const auto dir = GetForwardDirection();
		SetLocalPosition(position - dir * value);
	}

	void MoveRight(const float value)
	{
		const auto dir = GetRightDirection();
		SetLocalPosition(position + dir * value);
	}
	void MoveLeft(const float value)
	{
		const auto dir = GetRightDirection();
		SetLocalPosition(position - dir * value);
	}

	void MoveUp(const float value)
	{
		const auto dir = GetUpDirection();
		SetLocalPosition(position + dir * value);
	}
	void MoveDown(const float value)
	{
		const auto dir = GetUpDirection();
		SetLocalPosition(position - dir * value);
	}

	Vector3 GetUpDirection() const
	{
		Vector3 dir = XMVector3Rotate(Vector3::Up, GetWorldOrientation());
		dir.Normalize();
		return dir;
	}

	Vector3 GetRightDirection() const
	{
		Vector3 dir = XMVector3Rotate(Vector3::Right, GetWorldOrientation());
		dir.Normalize();
		return dir;
	}

	Vector3 GetForwardDirection() const
	{
		Vector3 dir = XMVector3Rotate(Vector3::Forward, GetWorldOrientation());
		dir.Normalize();
		return dir;
	}

	Quaternion GetWorldOrientation() const
	{
		return Quaternion::CreateFromYawPitchRoll(GetWorldRotation());
	}

	Quaternion GetLocalOrientation() const
	{
		return Quaternion::CreateFromYawPitchRoll(rotation);
	}

	void SetPositionX(const float x) { position.x = x; }
	void SetPositionY(const float y) { position.y = y; }
	void SetPositionZ(const float z) { position.z = z; }

	void RecalculateModel()
	{
		const auto worldPosition = GetWorldPosition();
		const auto worldRotation = GetWorldPosition();
		const auto worldScale = GetWorldPosition();

		if (worldPosition == prevPosition && worldRotation == prevRotation && worldScale == prevScale && parent != prevParent) return;

		prevPosition = worldPosition;
		prevRotation = worldRotation;
		prevScale = worldScale;
		prevParent = parent;

		ForceRecalculateModel();
	}

	void ForceRecalculateModel()
	{
		model = Matrix::CreateScale(GetWorldScale()) *
			Matrix::CreateFromQuaternion(GetWorldOrientation()) *
			Matrix::CreateTranslation(GetWorldPosition());
	}

private:

	std::set<Transform*> children;

	Transform* parent = nullptr;

	Matrix model = Matrix::Identity;

	Vector3 position = Vector3::Zero;
	Vector3 rotation = Vector3::Zero;
	Vector3 scale = Vector3::One;

	Vector3 prevPosition = Vector3::Zero;
	Vector3 prevRotation = Vector3::Zero;
	Vector3 prevScale = Vector3::One;
	Transform* prevParent;
};


