#include "pch.h"

#include "LevEngine.h"
#include "EntryPoint.h"
#include "FPSGame/FPSGame.h"

class SandboxApp : public Application
{
public:
	SandboxApp(const ApplicationCommandLineArgs args)
		: Application({ "Sandbox", 1200, 1200, args})
	{
		PushLayer(new Sandbox::FPSGame);
	}
	~SandboxApp() = default;
};

Application* CreateApplication(ApplicationCommandLineArgs args)
{
	return new SandboxApp(args);
}