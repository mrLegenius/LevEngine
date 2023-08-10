#pragma once
#include "PanelBase.h"
#include "Katamari/EntitySelection.h"
#include "Scene/Entity.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	public:
		explicit PropertiesPanel(const Ref<EntitySelection>& entitySelection) : m_Selection(entitySelection) { }
	protected:
		std::string GetName() override { return "Properties"; }
		void DrawContent() override;
	private:
		static void DrawVector3Control(const std::string& label, Vector3& values, float resetValue, float columnWidth);
		template <class T, class UIFunction>
		void DrawComponent(const std::string& label, Entity entity, UIFunction draw, bool removable);
		void DrawComponents(Entity entity);
		void DrawTexture2D(const std::string& label, std::function<void(const Ref<Texture>&)> onTextureLoaded);
		template <class T>
		void DrawAddComponent(const std::string& label) const;

		Ref<EntitySelection> m_Selection;
	};
}

