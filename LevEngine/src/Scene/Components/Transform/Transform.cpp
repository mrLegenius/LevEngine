#include "levpch.h"
#include "Transform.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	Transform::Transform() { ForceRecalculateModel(); }
	Transform::Transform(const Entity entity): entity(entity) { ForceRecalculateModel(); }

	Vector3 Transform::GetLocalPosition() const { return position; }
	void Transform::SetLocalPosition(const Vector3 value) { position = value; }

	Quaternion Transform::GetLocalRotation() const{ return rotation; }
	void Transform::SetLocalRotation(const Quaternion value) { rotation = value; }

	Vector3 Transform::GetLocalScale() const { return scale; }
	void Transform::SetLocalScale(const Vector3 value) { scale = value; }

	Vector3 Transform::GetWorldPosition() const
	{
		if (parent)
		{
			const auto& parentTransform = parent.GetComponent<Transform>();
			
			Vector3 worldPos = position;
			worldPos *= parentTransform.GetWorldScale();
			worldPos = Vector3::Transform(worldPos, parentTransform.GetWorldRotation());
			worldPos += parentTransform.GetWorldPosition();
			return worldPos;
		}
		return position;
	}

	Vector3 Transform::GetWorldScale() const
	{
		if (parent)
		{
			const auto& parentTransform = parent.GetComponent<Transform>();
			return scale * parentTransform.GetWorldScale();
		}

		return scale;
	}

	Quaternion Transform::GetWorldRotation() const
	{
		if (parent)
		{
			const auto& parentTransform = parent.GetComponent<Transform>();
			return rotation * parentTransform.GetWorldRotation();
		}

		return rotation;
	}

	void Transform::RemoveChild(const Entity entity)
	{
		const auto it = std::find(children.begin(), children.end(), entity);
		if (it != children.end())
			children.erase(it);
	}

	void Transform::SetParent(const Entity value, const bool keepWorldTransform)
	{
		if (entity == value)
		{
			Log::CoreWarning("{0} is trying to set itself as a parent", value.GetName());
			return;
		}

		if (parent == value) return;

		if (parent)
		{
			auto& parentTransform = parent.GetComponent<Transform>();
			parentTransform.RemoveChild(entity);
		}

		if (value)
		{
			auto& newParentTransform = value.GetComponent<Transform>();
			newParentTransform.children.emplace_back(entity);
		}

		if (!keepWorldTransform)
		{
			parent = value;
			return;
		}

		const auto position = GetWorldPosition();
		const auto rotation = GetWorldRotation();
		const auto scale = GetWorldScale();

		parent = value;

		SetWorldPosition(position);
		SetWorldRotation(rotation);
		SetWorldScale(scale);
	}

	void Transform::SetWorldPosition(const Vector3 value)
	{
		position = value;

		if (parent)
		{
			const auto& parentTransform = parent.GetComponent<Transform>();
				
			position -= parentTransform.GetWorldPosition();
			auto rot = parentTransform.GetWorldRotation();
			rot.Conjugate();
			position = Vector3::Transform(position, rot);

			const auto parentScale =  parentTransform.GetWorldScale();
			if (!Math::IsZero(parentScale.x))
				position.x /= parentScale.x;

			if (!Math::IsZero(parentScale.y))
				position.y /= parentScale.y;

			if (!Math::IsZero(parentScale.z))
				position.z /= parentScale.z;
		}
	}

	void Transform::SetWorldRotation(const Quaternion value)
	{
		rotation = value;

		if (parent)
		{
			const auto& parentTransform = parent.GetComponent<Transform>();
			rotation /= parentTransform.GetWorldRotation();
		}
	}

	void Transform::SetWorldScale(const Vector3 value)
	{
		if (parent)
		{
			const auto& parentTransform = parent.GetComponent<Transform>();
			const auto parentScale = parentTransform.GetWorldScale();
			scale.x = DirectX::XMScalarNearEqual(parentScale.x, 0, 0.0001f) ? 0 : value.x / parentScale.x;
			scale.y = DirectX::XMScalarNearEqual(parentScale.y, 0, 0.0001f) ? 0 : value.y / parentScale.y;
			scale.z = DirectX::XMScalarNearEqual(parentScale.z, 0, 0.0001f) ? 0 : value.z / parentScale.z;
		}
		else
		{
			scale = value;
		}
	}

	void Transform::MoveForward(const float value) { Move(GetForwardDirection() * value); }
	void Transform::MoveBackward(const float value) { Move(-GetForwardDirection() * value); }
	void Transform::MoveRight(const float value) { Move(GetRightDirection() * value); }
	void Transform::MoveLeft(const float value) { Move(-GetRightDirection() * value); }
	void Transform::MoveUp(const float value) { Move(GetUpDirection() * value); }
	void Transform::MoveDown(const float value) { Move(-GetUpDirection() * value); }

	Vector3 Transform::GetUpDirection() const
	{
		Vector3 dir = XMVector3Rotate(Vector3::Up, GetWorldRotation());
		dir.Normalize();
		return dir;
	}

	Vector3 Transform::GetRightDirection() const
	{
		Vector3 dir = XMVector3Rotate(Vector3::Right, GetWorldRotation());
		dir.Normalize();
		return dir;
	}

	Vector3 Transform::GetForwardDirection() const
	{
		Vector3 dir = XMVector3Rotate(Vector3::Forward, GetWorldRotation());
		dir.Normalize();
		return dir;
	}

	void Transform::RecalculateModel()
	{
		const auto worldPosition = GetWorldPosition();
		const auto worldRotation = GetWorldRotation();
		const auto worldScale = GetWorldScale();

		if (worldPosition == prevPosition && worldRotation == prevRotation && worldScale == prevScale && parent != prevParent) return;

		prevPosition = worldPosition;
		prevRotation = worldRotation;
		prevScale = worldScale;
		prevParent = parent;

		ForceRecalculateModel();
	}

	void Transform::ForceRecalculateModel()
	{
		model = Matrix::CreateScale(GetWorldScale()) *
			Matrix::CreateFromQuaternion(GetWorldRotation()) *
			Matrix::CreateTranslation(GetWorldPosition());
	}
	
	void Transform::CreateLuaBind(sol::state& lua)
	{
		lua.new_usertype <Transform>(
			"Transform",
			"type_id", &entt::type_hash<Transform>::value,
			sol::call_constructor,
			sol::factories(
				[]()
				{
					return Transform();
				},
				[](const Entity entity)
				{
					return Transform(entity);
				}),
			"position", &Transform::position,
			"getWorldPosition", [](Transform& transform) 
			{
				return transform.GetWorldPosition();
			},
			"setWorldPosition", [](Transform& transform, const Vector3& position)
			{
				return transform.SetWorldPosition(position);
			},
			"scale", &Transform::scale,
			"getWorldScale", [](Transform& transform)
			{
				return transform.GetWorldScale();
			},
			"setWorldScale", [](Transform& transform, const Vector3& scale)
			{
				return transform.SetWorldScale(scale);
			},
			"rotation", &Transform::rotation,
			"getWorldRotation", [](Transform& transform)
			{
				return transform.GetWorldRotation();
			},
			"setWorldRotation", [](Transform& transform, const Quaternion& rotation)
			{
				return transform.SetWorldRotation(rotation);
			}
		);
	}

	class TransformSerializer final : public ComponentSerializer<Transform, TransformSerializer>
	{
	protected:
		const char* GetKey() override { return "Transform"; }

		void SerializeData(YAML::Emitter& out, const Transform& component) override
		{
			out << YAML::Key << "Position" << YAML::Value << component.GetLocalPosition();
			out << YAML::Key << "Rotation" << YAML::Value << component.GetLocalRotation().ToEuler();
			out << YAML::Key << "Scale" << YAML::Value << component.GetLocalScale();
		}
		void DeserializeData(YAML::Node& node, Transform& component) override
		{
			component.SetLocalPosition(node["Position"].as<Vector3>());
			component.SetLocalRotation(Quaternion::CreateFromYawPitchRoll(node["Rotation"].as<Vector3>()));
			component.SetLocalScale(node["Scale"].as<Vector3>());
		}
	};
}
