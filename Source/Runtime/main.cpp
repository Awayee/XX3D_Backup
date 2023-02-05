#include "Engine.h"
#include "Core/macro.h"

int main() {
	// Run Editor
	{
		LOG("Runtime");
		Engine::XXEngine engine;
		engine.EngineRun();
	}
	return 0;
}