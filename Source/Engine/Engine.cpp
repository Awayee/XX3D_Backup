#include "Engine.h"
#include "Render/Window/WindowSystemGLFW.h"
#include "Core/macro.h"
#include "Resource/Config/Config.h"
#include "EngineContext.h"

namespace Engine {
	XXEngine::XXEngine() {
		auto* context = Context();
		// init window
		WindowInitInfo initInfo;
		initInfo.width = GetConfig()->GetWindowSize().w;
		initInfo.height = GetConfig()->GetWindowSize().h;
		initInfo.title = PROJECT_NAME;
		initInfo.resizeable = true;
		context->m_Window.reset(new WindowSystemGLFW);
		context->m_Window->Initialize(initInfo);

		// init renderer
		context->m_Renderer.reset(new RenderSystem(context->Window()));
		context->m_Renderer->SetEnable(true);
	}
	XXEngine::~XXEngine() {
		auto* context = Context();
		context->m_Window.reset();
		context->m_Renderer.reset();
	}

	bool XXEngine::Tick() {
		if(Context()->Window()->ShouldClose()) {
			Context()->Renderer()->SetEnable(false);
			return false;
		}

		Context()->Window()->PollEvents();
		Context()->Renderer()->Tick();
		return true;
	}
}
