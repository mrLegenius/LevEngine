#include "LevEngine.h"
#include "EntryPoint.h"

#include "EditorLayer.h"

class LetEditor : public Application
{
public:
	LetEditor() : Application("LevEditor", 1280, 720)
	{
		PushLayer(new Editor::EditorLayer);
	}
	~LetEditor() = default;
};

Application* CreateApplication()
{
	return new LetEditor;
}