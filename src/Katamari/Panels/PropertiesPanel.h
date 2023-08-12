#pragma once
#include "PanelBase.h"
#include "../EntitySelection.h"
#include "../ComponentDrawers/ComponentDrawer.h"
#include "Scene/Entity.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	public:
		explicit PropertiesPanel(const Ref<EntitySelection>& entitySelection);

	protected:
		std::string GetName() override { return "Properties"; }
		void DrawContent() override;
	private:
		void DrawComponents(Entity entity) const;

		Ref<EntitySelection> m_Selection;

		std::vector<Ref<IComponentDrawer>> m_ComponentDrawers;
	};
}

