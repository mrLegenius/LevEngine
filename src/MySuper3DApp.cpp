#include "MySuper3DLayer.h"
#include "PongLayer.h"
#include "Kernel/Application.h"

int main()
{
	Application app("My3DApp", 800, 800);
	app.PushLayer(new PongLayer);

	app.Run();
}
