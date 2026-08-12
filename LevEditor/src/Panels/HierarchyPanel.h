#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
	class HierarchyPanel final : public Panel
	{
	public:
		//<--- entityFocused is raised when an entity is double clicked, so the scene cameras can be brought to it ---<<
		explicit HierarchyPanel(Action<Entity> entityFocused = {})
			: m_EntityFocused(Move(entityFocused))
		{
			m_DefaultWindowSize = Vector2{ 230, 660 };
		}

		bool OnKeyPressed(KeyPressedEvent& e) override;

	protected:
		String GetName() override { return "Hierarchy"; }
		void DrawContent() override;
		static void CreatePrefab(Entity entity, const Path& path);
		static void SavePrefab(Entity entity, const Path& path);

	private:
		void DrawEntityNode(Entity entity);
		void SelectEntity(Entity entity);

		Action<Entity> m_EntityFocused;
		Vector<Entity> m_EntitiesToDelete;
		Entity m_PressedEntity;
	};
}
