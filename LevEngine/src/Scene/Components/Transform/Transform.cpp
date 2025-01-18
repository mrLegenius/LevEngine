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
		
		uint32_t childIndex = it->GetComponent<Transform>().GetChildIndex();
		children.erase(it);

		//Move other children indices 
		for (auto child : children)
		{
			auto& childTransform = child.GetComponent<Transform>();
			if (childTransform.GetChildIndex() > childIndex)
				childTransform.childIndex--;
		}
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
		
		auto& parentTransform = parent.GetComponent<Transform>();
		parentTransform.RemoveChild(entity);

		auto& newParentTransform = value.GetComponent<Transform>();
		childIndex = newParentTransform.children.size();
		newParentTransform.children.emplace_back(entity);
		depth = newParentTransform.GetHierarchyDepth() + 1;

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
	
	void Transform::SetChildIndex(uint16_t index)
	{
		if (!parent)
		{
			Log::CoreWarning("Can't set child index for root object");
			return;
		}

		if (childIndex == index) return;

		auto& parentTransform = parent.GetComponent<Transform>();
		auto lastChildIndex = Math::Max(parentTransform.GetChildrenCount() - 1, 0);
		index = Math::Min(static_cast<int>(index), lastChildIndex);

		//Move other children indices 
		uint16_t min = Math::Min(index, childIndex);
		uint16_t max = Math::Max(index, childIndex);
		
		const auto& children = parentTransform.GetChildren();
		bool indexIncreased = index > childIndex;
		auto offset = indexIncreased ? -1 : +1;
		
		for (int i = min; i <= max; ++i)
		{
			auto child = children[i];
			auto& childTransform = child.GetComponent<Transform>();
			childTransform.childIndex += offset;
		}

		parentTransform.children.erase(parentTransform.children.begin() + childIndex - 1);
		parentTransform.children.emplace(parentTransform.children.begin() + index, entity);
		
		childIndex = index;
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

	//TODO: Remove sorting and replace with inserting to vector instead in serializator
	void Transform::SortChildren()
	{
		std::ranges::sort(children, [](const Entity& lhs, const Entity& rhs)
		{
			auto leftTransform = lhs.GetComponent<Transform>();
			auto rightTransform = rhs.GetComponent<Transform>();

			return leftTransform.childIndex < rightTransform.childIndex;
		});
	}
}
