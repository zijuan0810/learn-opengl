#include "Engine.h"

namespace ox
{
	Engine* Engine::Ins()
	{
		static Engine engine;
		return &engine;
	}
}