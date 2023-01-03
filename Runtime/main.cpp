#include "Core/macro.h"
#include "Engine/Engine.h"


int main() {
	// Run Editor
	{
		LOG("Runtime");
		Engine::XXEngine engine;
		engine.EngineRun();
	}
	return 0;
}