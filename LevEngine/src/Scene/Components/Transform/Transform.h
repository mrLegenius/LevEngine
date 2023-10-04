#pragma once
#include "DataTypes/Vector.h"
#include "Scene/Entity.h"
#include "Math/Math.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(Transform);
	
	struct Transform
	{
		Transform()
		{
			ForceRecalculateModel();
		}

		Transform(const Entity entity) : entity(entity)
		{
			ForceRecalculateModel();
		}

		[[nodiscard]] const Matrix& GetModel() const { return model; }

		[[nodiscard]] Vector3 GetLocalPosition() const { return position; }
		[[nodiscard]] Vector3 GetLocalScale() const { return scale; }
		[[nodiscard]] Quaternion GetLocalRotation() const { return rotation; }

		[[nodiscard]] Vector3 GetWorldPosition() const
		{
			Vector3 localPos = position;
			if (parent)
			{
				const auto& parentTransform = parent.GetComponent<Transform>();

				localPos *= parentTransform.GetWorldScale();
				localPos = Vector3::Transform(localPos, parentTransform.GetWorldRotation());
				localPos += parentTransform.GetWorldPosition();
			}
			return localPos;
		}

		[[nodiscard]] Vector3 GetWorldScale() const
		{
			if (parent)
			{
				const auto& parentTransform = parent.GetComponent<Transform>();
				return scale * parentTransform.GetWorldScale();
			}

			return scale;
		}
		[[nodiscard]] Quaternion GetWorldRotation() const
		{
			if (parent)
			{
				const auto& parentTransform = parent.GetComponent<Transform>();
				return rotation * parentTransform.GetWorldRotation();
			}

			return rotation;
		}

		[[nodiscard]] const Vector<Entity>& GetChildren() const { return children; }
		[[nodiscard]] uint32_t GetChildrenCount() const { return children.size(); }

		[[nodiscard]] Entity GetParent() const { return parent; }
		void RemoveChild(const Entity entity)
		{
			const auto it = std::find(children.begin(), children.end(), entity);
			if (it != children.end())
				children.erase(it);
		}
		void SetParent(Entity value, bool keepWorldTransform = true);

		void SetLocalPosition(const Vector3 value) { position = value; }
		void SetWorldPosition(const Vector3 value)
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

		void SetWorldRotation(const Quaternion value)
		{
			rotation = value;

			if (parent)
			{
				const auto& parentTransform = parent.GetComponent<Transform>();
				rotation /= parentTransform.GetWorldRotation();
			}
		}

		void SetLocalRotation(const Quaternion value)
		{
			rotation = value;
		}

		void SetLocalScale(const Vector3 value)
		{
			scale = value;
		}

		void SetWorldScale(const Vector3 value)
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

		[[nodiscard]] Vector3 GetUpDirection() const
		{
			Vector3 dir = XMVector3Rotate(Vector3::Up, GetWorldRotation());
			dir.Normalize();
			return dir;
		}

		[[nodiscard]] Vector3 GetRightDirection() const
		{
			Vector3 dir = XMVector3Rotate(Vector3::Right, GetWorldRotation());
			dir.Normalize();
			return dir;
		}

		[[nodiscard]] Vector3 GetForwardDirection() const
		{
			Vector3 dir = XMVector3Rotate(Vector3::Forward, GetWorldRotation());
			dir.Normalize();
			return dir;
		}

		void SetPositionX(const float x) { position.x = x; }
		void SetPositionY(const float y) { position.y = y; }
		void SetPositionZ(const float z) { position.z = z; }

		void RecalculateModel()
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

		void ForceRecalculateModel()
		{
			model = Matrix::CreateScale(GetWorldScale()) *
				Matrix::CreateFromQuaternion(GetWorldRotation()) *
				Matrix::CreateTranslation(GetWorldPosition());
		}

	private:

		Vector<Entity> children;

		Entity parent{};

		Matrix model = Matrix::Identity;

		Vector3 position = Vector3::Zero;
		Quaternion rotation = Quaternion::Identity;
		Vector3 scale = Vector3::One;

		Vector3 prevPosition = Vector3::Zero;
		Quaternion prevRotation = Quaternion::Identity;
		Vector3 prevScale = Vector3::One;
		Entity prevParent{};
		Entity entity{};
	};
}

