#pragma once
#include "PanelBase.h"
#include "Components/ComponentDrawer.h"
#include "Katamari/EntitySelection.h"
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
		void DrawTexture2D(const std::string& label, std::function<void(const Ref<Texture>&)> onTextureLoaded);
		template <class T>
		void DrawAddComponent(const std::string& label) const;

		Ref<EntitySelection> m_Selection;

		std::vector<Ref<IComponentDrawer>> m_ComponentDrawers;
	};
}

