#pragma once
#include "Panel.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	protected:
		std::string GetName() override { return "Properties"; }
		void DrawContent() override;
	};
}

