﻿#include "pch.h"
#include "PropertiesPanel.h"

namespace LevEngine::Editor
{
void PropertiesPanel::DrawContent()
{
	if (Selection::Current())
		Selection::Current()->DrawProperties();
}
}
