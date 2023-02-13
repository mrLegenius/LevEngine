#include "MySuper3DLayer.h"
#include "Kernel/Application.h"

int main()
{
	Application app("My3DApp", 800, 800);
	app.PushLayer(new MySuper3DLayer);

	app.Run();
}
