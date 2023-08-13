#pragma once
#include "PanelBase.h"
#include "../EntitySelection.h"

namespace LevEngine::Editor
{
	class PropertiesPanel final : public Panel
	{
	protected:
		std::string GetName() override { return "Properties"; }
		void DrawContent() override;
	};
}

