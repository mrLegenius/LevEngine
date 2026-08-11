#include "levpch.h"
#include "Transform.h"

#include "../ComponentSerializer.h"

namespace LevEngine
{
	Transform::Transform() { ForceRecalculateModel(); }

	Transform::Transform(const Entity entity, Entity parent)
		: parent(parent), entity(entity)
	{
		if (parent)
		{
			auto& parentTransform = parent.GetComponent<Transform>();
			parentTransform.children.emplace_back(entity);
			childIndex = parentTransform.children.size() - 1;
			depth = parentTransform.GetHierarchyDepth() + 1;
		}
		
		ForceRecalculateModel();
	}

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
		const auto it = std::ranges::find(children, entity);

		if (it == children.end()) return;

		children.erase(it);

		//<--- Children are always stored in childIndex order ---<<
		for (uint16_t i = 0; i < children.size(); ++i)
			children[i].GetComponent<Transform>().childIndex = i;
	}

	void Transform::SetParent(const Entity value, const bool keepWorldTransform)
	{
		if (!value)
		{
			Log::CoreWarning("Can't set invalid entity as a parent");
			return;
		}
		
		if (!parent)
		{
			Log::CoreWarning("Can't set parent for root object");
			return;
		}

		if (parent == value) return;
		
		if (entity == value)
		{
			Log::CoreWarning("{0} is trying to set itself as a parent", value.GetName());
			return;
		}

		Queue<Entity> childrenToCheck;

		for (auto child : children)
			childrenToCheck.push(child);

		while (!childrenToCheck.empty())
		{
			auto child = childrenToCheck.front();
			
			if (child == value)
			{
				Log::CoreWarning("{0} is trying to set a child as a parent", entity.GetName());
				return;
			}

			for (auto subChild : child.GetComponent<Transform>().GetChildren())
				childrenToCheck.push(subChild);
			
			childrenToCheck.pop();
		}
		
		const bool wasAttached = static_cast<bool>(parent);

		//<--- An entity without a parent is not attached to the hierarchy yet, there is nothing to detach from ---<<
		if (wasAttached)
			parent.GetComponent<Transform>().RemoveChild(entity);

		auto& newParentTransform = value.GetComponent<Transform>();
		auto& siblings = newParentTransform.children;

		//<--- A detached entity keeps the index it was given, a reparented one goes to the end ---<<
		const auto siblingsCount = static_cast<uint16_t>(siblings.size());
		const auto newIndex = wasAttached ? siblingsCount : Math::Min(childIndex, siblingsCount);

		siblings.insert(siblings.begin() + newIndex, entity);
		depth = newParentTransform.GetHierarchyDepth() + 1;

		//<--- Children are always stored in childIndex order ---<<
		for (uint16_t i = 0; i < siblings.size(); ++i)
			siblings[i].GetComponent<Transform>().childIndex = i;

		//<--- The whole subtree moved with us ---<<
		UpdateChildrenDepth();

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
	
	void Transform::SetChildIndex(const uint16_t index)
	{
		if (!parent)
		{
			//<--- Not attached to a hierarchy yet, keep the index until a parent is set ---<<
			childIndex = index;
			return;
		}

		auto& parentTransform = parent.GetComponent<Transform>();
		auto& siblings = parentTransform.children;

		const auto it = std::ranges::find(siblings, entity);
		if (it == siblings.end()) return;

		const auto currentIndex = static_cast<uint16_t>(eastl::distance(siblings.begin(), it));
		const auto lastChildIndex = static_cast<uint16_t>(siblings.size() - 1);
		const auto newIndex = Math::Min(index, lastChildIndex);

		if (currentIndex != newIndex)
		{
			siblings.erase(it);
			siblings.insert(siblings.begin() + newIndex, entity);
		}

		//<--- Children are always stored in childIndex order ---<<
		for (uint16_t i = 0; i < siblings.size(); ++i)
			siblings[i].GetComponent<Transform>().childIndex = i;
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
			scale.x = Math::IsZero(parentScale.x) ? 0 : value.x / parentScale.x;
			scale.y = Math::IsZero(parentScale.y) ? 0 : value.y / parentScale.y;
			scale.z = Math::IsZero(parentScale.z) ? 0 : value.z / parentScale.z;
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

	void Transform::UpdateChildrenDepth()
	{
		for (auto child : children)
		{
			auto& childTransform = child.GetComponent<Transform>();
			childTransform.depth = depth + 1;
			childTransform.UpdateChildrenDepth();
		}
	}

	void Transform::SortChildren()
	{
		std::ranges::sort(children, [](const Entity& lhs, const Entity& rhs)
		{
			const auto& leftTransform = lhs.GetComponent<Transform>();
			const auto& rightTransform = rhs.GetComponent<Transform>();

			return leftTransform.childIndex < rightTransform.childIndex;
		});

		//<--- Children are always stored in childIndex order ---<<
		for (uint16_t i = 0; i < children.size(); ++i)
			children[i].GetComponent<Transform>().childIndex = i;
	}
}
