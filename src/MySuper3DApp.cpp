#include "pch.h"
#include "Katamari/KatamariLayer.h"
#include "Kernel/Application.h"

int main()
{
	Log::Logger::Init();

	LEV_PROFILE_BEGIN_SESSION("Startup", "LevEngineProfile-Startup.json");

	const auto app = new Application("My3DApp", 1280, 720);
	app->PushLayer(new KatamariLayer);
	LEV_PROFILE_END_SESSION();

	LEV_PROFILE_BEGIN_SESSION("Runtime", "LevEngineProfile-Runtime.json");
	app->Run();
	LEV_PROFILE_END_SESSION();

	LEV_PROFILE_BEGIN_SESSION("Shutdown", "LevEngineProfile-Shutdown.json");
	delete app;
	LEV_PROFILE_END_SESSION();
}
