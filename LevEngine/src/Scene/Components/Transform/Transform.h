#pragma once
#include "Scene/Entity.h"
#include "Math/Math.h"
#include "Scene/Components/TypeParseTraits.h"

namespace LevEngine
{
	REGISTER_PARSE_TYPE(Transform);
	
	struct Transform
	{
		Transform();

		explicit Transform(Entity entity);

		[[nodiscard]] const Matrix& GetModel() const { return model; }
		
		[[nodiscard]] Vector3 GetLocalPosition() const;
		void SetLocalPosition(Vector3 value);
		
		[[nodiscard]] Quaternion GetLocalRotation() const;
		void SetLocalRotation(const Quaternion value);

		[[nodiscard]] Vector3 GetLocalScale() const;
		void SetLocalScale(const Vector3 value);

		[[nodiscard]] Vector3 GetWorldPosition() const;
		void SetWorldPosition(Vector3 value);
		
		[[nodiscard]] Quaternion GetWorldRotation() const;
		void SetWorldRotation(Quaternion value);

		[[nodiscard]] Vector3 GetWorldScale() const;
		void SetWorldScale(Vector3 value);

		[[nodiscard]] Entity GetParent() const { return parent; }
		void SetParent(Entity value, bool keepWorldTransform = true);
		
		[[nodiscard]] const Vector<Entity>& GetChildren() const { return children; }
		[[nodiscard]] uint32_t GetChildrenCount() const { return (uint32_t)children.size(); }
		void RemoveChild(Entity entity);
		
		void Move(const Vector3 value) { position += value; }
		void MoveForward(float value);
		void MoveBackward(float value);
		void MoveRight(float value);
		void MoveLeft(float value);
		void MoveUp(float value);
		void MoveDown(float value);
		
		[[nodiscard]] Vector3 GetUpDirection() const;
		[[nodiscard]] Vector3 GetRightDirection() const;
		[[nodiscard]] Vector3 GetForwardDirection() const;

		void SetPositionX(const float x) { position.x = x; }
		void SetPositionY(const float y) { position.y = y; }
		void SetPositionZ(const float z) { position.z = z; }

		void RecalculateModel();
		void ForceRecalculateModel();

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

