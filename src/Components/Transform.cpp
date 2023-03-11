#include "Transform.h"

void Transform::SetParent(Transform* value, const bool keepWorldTransform)
{
	if (parent == value) return;

	if (parent)
		parent->children.erase(this);

	if (value)
		value->children.insert(this);

	if (!keepWorldTransform)
	{
		parent = value;
		return;
	}

	const auto position = GetWorldPosition();
	const auto rotation = GetWorldOrientation();
	const auto scale = GetWorldScale();

	parent = value;

	SetWorldPosition(position);
	SetWorldRotation(rotation);
	SetWorldScale(scale);
}
