#include "src/EditorLayer.h"
#include "EntryPoint.h"

class Sandbox : public LevEngine::Application
{
public:
    Sandbox() : Application("Sandbox")
    {
        PushLayer(new LevEngine::EditorLayer);
    }
    ~Sandbox() = default;
};

LevEngine::Application* LevEngine::CreateApplication()
{
    return new Sandbox;
}