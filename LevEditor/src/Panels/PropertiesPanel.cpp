#include "pch.h"
#include "PropertiesPanel.h"

#include "Selection.h"

namespace LevEngine::Editor
{
void PropertiesPanel::DrawContent()
{
	LEV_PROFILE_FUNCTION();
	
	if (Selection::Current())
		Selection::Current()->DrawProperties();
}
}
