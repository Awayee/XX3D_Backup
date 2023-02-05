#pragma once
#include "Render/Window/WindowSystem.h"
#include "Render/Common/RenderSystem.h"
#include "Core/Time/Time.h"

namespace Engine {
	class XXEngine {
	private:
		WindowSystemBase* m_Window {nullptr};
		RenderSystem* m_Renderer {nullptr};
		TimePoint m_LastTickTime{ std::chrono::steady_clock::now() };

		uint32 m_FPSFrameCounter{0U};
		float m_FPSDurationMs{0.0f};
		float m_FPS{0.0f};
	public:
		XXEngine();
		~XXEngine();
		void EngineRun();
		bool Tick();
		float GetFPS() const { return m_FPS; }
		WindowSystemBase* GetWindowSystem() const { return m_Window; }
		RenderSystem* GetRenderSystem() const { return m_Renderer; }

	private:
		float ComputeDeltaTime();
		void ComputeFPS(float deltaTime);
	};
}