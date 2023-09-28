#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
	class HierarchyPanel final : public Panel
	{
	public:
		HierarchyPanel() = default;
		
		bool OnKeyPressed(KeyPressedEvent& e) override;

	protected:
		String GetName() override { return "Hierarchy"; }
		void DrawContent() override;
		static void CreatePrefab(Entity entity, const Path& path);

	private:
		void DrawEntityNode(Entity entity);
		
		Vector<Entity> m_EntitiesToDelete;
	};
}
