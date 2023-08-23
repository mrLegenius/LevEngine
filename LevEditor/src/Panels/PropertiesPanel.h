#pragma once
#include "PanelBase.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	public:
		PropertiesPanel()
		{
			m_WindowPadding = Vector2{ 10, 5 };
		}
	protected:
		std::string GetName() override { return "Properties"; }
		void DrawContent() override;
	};
}

