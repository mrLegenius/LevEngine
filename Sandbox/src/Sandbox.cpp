#include "pch.h"

#include "EntryPoint.h"
#include "FPSGame/FPSGame.h"

class SandboxApp : public LevEngine::Application
{
public:
	SandboxApp(const LevEngine::ApplicationCommandLineArgs args)
		: Application({ "Sandbox", 1600, 900, args})
	{
		PushLayer(new Sandbox::FPSGame);
	}
	~SandboxApp() = default;
};

LevEngine::Application* CreateApplication(LevEngine::ApplicationCommandLineArgs args)
{
	return new SandboxApp(args);
}