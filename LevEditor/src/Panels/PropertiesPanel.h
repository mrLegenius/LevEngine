#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	protected:
		String GetName() override { return "Properties"; }
		void DrawContent() override;
	};
}

