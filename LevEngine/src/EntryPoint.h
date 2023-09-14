#pragma once

extern Application* CreateApplication(ApplicationCommandLineArgs);

int main(int argc, char* argv[])
{
	LevEngine::Log::Logger::Init();

	LEV_PROFILE_BEGIN_SESSION("Startup", "LevEngineProfile-Startup.json");

	auto app = CreateApplication({argc, argv});
	LEV_PROFILE_END_SESSION();

	LEV_PROFILE_BEGIN_SESSION("Runtime", "LevEngineProfile-Runtime.json");
	app->Run();
	LEV_PROFILE_END_SESSION();

	LEV_PROFILE_BEGIN_SESSION("Shutdown", "LevEngineProfile-Shutdown.json");
	delete app;
	LEV_PROFILE_END_SESSION();
}