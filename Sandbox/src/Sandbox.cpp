#include "pch.h"

#include "LevEngine.h"
#include "EntryPoint.h"
#include "Game.h"

class SandboxApp : public Application
{
public:
	SandboxApp() : Application("Sandbox", 1600, 900)
	{
		PushLayer(new Sandbox::Game);
	}
	~SandboxApp() = default;
};

Application* CreateApplication()
{
	return new SandboxApp;
}