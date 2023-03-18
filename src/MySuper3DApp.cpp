#include "Debug/Profiler.h"
#include "FirstSteps/MySuper3DLayer.h"
#include "Pong/PongLayer.h"
#include "SolarSystem/SolarSystemLayer.h"
#include "Katamari/KatamariLayer.h"
#include "Kernel/Application.h"

int main()
{
	LEV_PROFILE_BEGIN_SESSION("Startup", "LevEngineProfile-Startup.json");

	Application app("My3DApp", 800, 800);
	//app.PushLayer(new MySuper3DLayer);
	//app.PushLayer(new PongLayer);
	//app.PushLayer(new SolarSystemLayer);
	app.PushLayer(new KatamariLayer);
	LEV_PROFILE_END_SESSION();

	LEV_PROFILE_BEGIN_SESSION("Runtime", "LevEngineProfile-Runtime.json");
	app.Run();
	LEV_PROFILE_END_SESSION();
}
