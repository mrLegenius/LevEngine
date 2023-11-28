#include "LevEngine.h"
#include "EntryPoint.h"

#include "EditorLayer.h"

class LevEditor : public LevEngine::Application
{
public:
	explicit LevEditor(const LevEngine::ApplicationCommandLineArgs args)
		: Application({ "LevEditor", 1280, 720, args})
	{
		PushLayer(new LevEngine::Editor::EditorLayer);
	}
	~LevEditor() = default;
};

LevEngine::Application* CreateApplication(const LevEngine::ApplicationCommandLineArgs args)
{
	return new LevEditor(args);
}