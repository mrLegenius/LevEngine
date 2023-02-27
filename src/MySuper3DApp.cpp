#include "FirstSteps/MySuper3DLayer.h"
#include "Pong/PongLayer.h"
#include "SolarSystem/SolarSystemLayer.h"
#include "Kernel/Application.h"

int main()
{
	Application app("My3DApp", 800, 800);
	//app.PushLayer(new MySuper3DLayer);
	//app.PushLayer(new PongLayer);
	app.PushLayer(new SolarSystemLayer);

	app.Run();
}
