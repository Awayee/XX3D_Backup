#pragma once
#include "Render/Window/WindowSystem.h"
#include "Render/Common/RenderSystem.h"
#include "Core/Time/Time.h"

namespace Engine {
	class XXEngine {
	private:
		TUniquePtr<WindowSystemBase> m_Window {nullptr};
		TUniquePtr<RenderSystem> m_Renderer{ nullptr };
	public:
		XXEngine();
		~XXEngine();
		bool Tick();
		WindowSystemBase* Window() const { return m_Window.get(); }
		RenderSystem* Renderer() const { return m_Renderer.get(); }
	};
}