#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	public:
		PropertiesPanel() { m_DefaultWindowSize = Vector2{ 555, 660 }; }

	protected:
		String GetName() override { return "Properties"; }
		void DrawContent() override;
	};
}

