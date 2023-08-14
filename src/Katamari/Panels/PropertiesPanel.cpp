#include "pch.h"
#include "PropertiesPanel.h"

#include "Katamari/Selection.h"

namespace LevEngine::Editor
{
void PropertiesPanel::DrawContent()
{
	if (Selection::Current())
		Selection::Current()->DrawProperties();
}
}
