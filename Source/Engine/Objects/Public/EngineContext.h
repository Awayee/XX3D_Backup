#pragma once
#include "Core/Public/TPtr.h"

namespace Engine {
	class Wnd;
}

namespace Engine {

	class Renderer;

	class EngineContext {
	private:
		friend class XXEngine;
		Engine::Wnd* m_Window;
		TUniquePtr<Renderer> m_Renderer;
	public:
		Engine::Wnd* Window() { return m_Window; }
		Renderer* Renderer() { return m_Renderer.get(); }
	};

	EngineContext* Context();
}