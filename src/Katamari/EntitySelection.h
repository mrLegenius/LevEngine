#pragma once
#include "Scene/Entity.h"

namespace LevEngine::Editor
{
	class EntitySelection
	{
	public:
		[[nodiscard]] Entity Get() const { return m_SelectionContext; }
		void Set(const Entity entity) { m_SelectionContext = entity; }
	private:
		Entity m_SelectionContext;
	};
}
