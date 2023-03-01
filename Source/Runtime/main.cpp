#include "Engine.h"
#include "EngineContext.h"
#include "Core/macro.h"
#include "Resource/Config/Config.h"

int main() {
	// Run Editor
	{
		LOG("Runtime");
		Engine::XXEngine engine;
		Engine::Context()->Renderer()->SetRenderArea({ 0, 0, GetConfig()->GetWindowSize().w, GetConfig()->GetWindowSize().h });
		while (engine.Tick());
	}
	return 0;
}
