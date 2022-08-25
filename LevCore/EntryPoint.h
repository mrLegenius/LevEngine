#pragma once
#include "src/Kernel/Logger.h"
#include "src/Kernel/Application.h"
#include "src/Debug/Instrumentor.h"
#define LEV_PLATFORM_WINDOWS 1
#ifdef LEV_PLATFORM_WINDOWS

extern LevEngine::Application* LevEngine::CreateApplication();

int main(int argc, char* argv[])
{
    LevEngine::Log::Logger::Init();

	LEV_PROFILE_BEGIN_SESSION("Startup", "LevEngineProfile-Startup.json");
	auto app = LevEngine::CreateApplication();
	LEV_PROFILE_END_SESSION();

	LEV_PROFILE_BEGIN_SESSION("Runtime", "LevEngineProfile-Runtime.json");
	app->Run();
	LEV_PROFILE_END_SESSION();

	LEV_PROFILE_BEGIN_SESSION("Shutdown", "LevEngineProfile-Shutdown.json");
	delete app;
	LEV_PROFILE_END_SESSION();
}

#endif