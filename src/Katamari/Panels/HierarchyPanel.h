#pragma once
#include "PanelBase.h"
#include "Katamari/EntitySelection.h"
#include "Scene/Entity.h"

namespace LevEngine::Editor
{
	class HierarchyPanel final : public Panel
	{
	public:
		HierarchyPanel() = default;
		HierarchyPanel(const Ref<Scene>& scene, const Ref<EntitySelection>& selection);

		void SetContext(const Ref<Scene>& scene);

	protected:
		std::string GetName() override { return "Hierarchy"; }
		void DrawContent() override;
	private:
		void DrawEntityNode(Entity entity) const;

		Ref<Scene> m_Context = nullptr;
		Ref<EntitySelection> m_Selection;

		friend class Scene;
	};
}
