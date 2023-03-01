#include "EngineContext.h"
#include "Render/Window/WindowSystem.h"
#include "Render/Common/RenderSystem.h"

namespace Engine {

	EngineContext* Context() {
		static EngineContext s_EngineContext;
		return &s_EngineContext;
	}

}