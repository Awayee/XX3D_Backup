#include "Engine.h"
#include "Engine/Window/WindowSystemGLFW.h"
#include "Core/macro.h"
#include <string>

namespace Engine {
	XXEngine::XXEngine()
	{
		m_Window = new WindowSystemGLFW();
		WindowInitInfo initInfo;
		initInfo.width = 1024;
		initInfo.height = 768;
		initInfo.title = PROJECT_NAME;
		initInfo.resizeable = true;
		m_Window->Initialize(initInfo);


		m_Renderer = new RenderSystem(m_Window);
		m_Renderer->SetEnable(true);
	}
	XXEngine::~XXEngine()
	{
		delete m_Renderer;
		delete m_Window;
	}
	void XXEngine::EngineRun()
	{
		while (true) {
			if(!Tick()) {
				return;
			}
		}
	}

	float XXEngine::ComputeDeltaTime()
	{
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		std::chrono::duration<float, std::milli> timeDuration = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_LastTickTime);
		m_LastTickTime = now;
		return timeDuration.count();
	}

	void XXEngine::ComputeFPS(float deltaTime)
	{
		m_FPSDurationMs += deltaTime;
		++m_FPSFrameCounter;
		if(m_FPSDurationMs > 1000.0f) {
			m_FPS = (float)m_FPSFrameCounter / (m_FPSDurationMs * 0.001f);
			m_FPSFrameCounter = 0u;
			m_FPSDurationMs = 0.0f;
		}
	}

	bool XXEngine::Tick()
	{
		if(m_Window->ShouldClose()) {
			m_Renderer->SetEnable(false);
			return false;
		}

		m_Window->PollEvents();
		m_Renderer->Tick();

		float deltaTime = ComputeDeltaTime();
		ComputeFPS(deltaTime);
		return true;
	}
}