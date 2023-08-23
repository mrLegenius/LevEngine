#pragma once
#include "Selection.h"

namespace LevEngine::Editor
{
	class EntitySelection final : public Selection
	{
	public:
		EntitySelection() = default;
		explicit EntitySelection(const Entity entity) : m_Entity(entity) { }

		[[nodiscard]] Entity Get() const { return m_Entity; }
		void Set(const Entity entity) { m_Entity = entity; }
		void DrawProperties() override;

	private:
		Entity m_Entity;
	};
}
