#include "Application.h"
#include "MySuper3DLayer.h"

int main()
{
	Application app("My3DApp", 800, 800);
	app.PushLayer(new MySuper3DLayer);

	app.Run();
}
