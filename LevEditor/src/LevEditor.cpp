﻿#include "LevEngine.h"
#include "EntryPoint.h"

#include "EditorLayer.h"

class LetEditor : public Application
{
public:
	LetEditor(const ApplicationCommandLineArgs args)
		: Application({ "LevEditor", 1600, 900, args})
	{
		PushLayer(new Editor::EditorLayer);
	}
	~LetEditor() = default;
};

Application* CreateApplication(ApplicationCommandLineArgs args)
{
	return new LetEditor(args);
}