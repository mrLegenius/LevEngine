#pragma once
#include "Panel.h"
#include "Scene/Scene.h"

namespace LevEngine::Editor
{
	class HierarchyPanel final : public Panel
	{
	public:
		HierarchyPanel() = default;
		HierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

		bool OnKeyPressed(KeyPressedEvent& e) override;

	protected:
		String GetName() override { return "Hierarchy"; }
		void DrawContent() override;
	private:
		void DrawEntityNode(Entity entity);

		Ref<Scene> m_Context = nullptr;

		Vector<Entity> m_EntitiesToDelete;

		friend class Scene;
	};
}
