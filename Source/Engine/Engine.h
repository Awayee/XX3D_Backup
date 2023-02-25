#pragma once
#include "Render/Window/WindowSystem.h"
#include "Render/Common/RenderSystem.h"
#include "Core/Time/Time.h"

namespace Engine {
	class XXEngine {
	private:
		TUniquePtr<WindowSystemBase> m_Window {nullptr};
		TUniquePtr<RenderSystem> m_Renderer{ nullptr };
		TimePoint m_LastTickTime{ std::chrono::steady_clock::now() };

		uint32 m_FPSFrameCounter{0U};
		float m_FPSDurationMs{0.0f};
		float m_FPS{0.0f};
	public:
		XXEngine();
		~XXEngine();
		bool Tick();
		float GetFPS() const { return m_FPS; }
		WindowSystemBase* Window() const { return m_Window.get(); }
		RenderSystem* Renderer() const { return m_Renderer.get(); }

	private:
		float ComputeDeltaTime();
		void ComputeFPS(float deltaTime);
	};
}