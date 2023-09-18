#include "pch.h"

#include "LevEngine.h"
#include "EntryPoint.h"
#include "Game.h"

class SandboxApp : public Application
{
public:
	SandboxApp(const ApplicationCommandLineArgs args)
		: Application({ "Sandbox", 1600, 900, args})
	{
		PushLayer(new Sandbox::Game);
	}
	~SandboxApp() = default;
};

Application* CreateApplication(ApplicationCommandLineArgs args)
{
	return new SandboxApp(args);
}