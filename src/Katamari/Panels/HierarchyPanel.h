#pragma once
#include "PanelBase.h"
#include "Scene/Entity.h"

namespace LevEngine::Editor
{
	class HierarchyPanel final : public Panel
	{
	public:
		HierarchyPanel() = default;
		HierarchyPanel(const Ref<Scene>& scene);

		void SetContext(const Ref<Scene>& scene);

	protected:
		std::string GetName() override { return "Hierarchy"; }
		void DrawContent() override;
	private:
		void DrawEntityNode(Entity entity);

		Ref<Scene> m_Context = nullptr;
		Entity m_SelectionContext{};

		friend class Scene;
	};
}
