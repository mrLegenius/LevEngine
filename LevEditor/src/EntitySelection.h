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

		//<--- Reuses the current entity selection, so everything holding a reference to it keeps working ---<<
		static void SelectEntity(Entity entity);

	private:
		Entity m_Entity;
	};
}
