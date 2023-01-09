//#include "satan/Satan.h"
#include "ox/Core/Engine.h"
#include "MyScene.h"


int main(int argc, char* argv[])
{
	ox::Engine::Ins()->Init(new MyScene());

	while (!ox::Engine::Ins()->WillClose())
		ox::Engine::Ins()->Run();

	glfwTerminate();

	return 0;
}