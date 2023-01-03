#pragma once
#include "UIBase.h"
#include "RHI/RHIInstance.h"
#include "UIRenderer.h"


namespace Engine {
	class WindowSystemBase;

	class RenderSystem {
	private:
		//RenderPasses
		RHI::RRenderPass* m_MainPass{nullptr};
		UIRenderer m_UIRenderer;
		TVector<RHI::RFramebuffer*> m_SwapchianFramebuffers;
		uint8_t m_CurrentFrameIndex{0};
	public:
		RenderSystem() = default;
		RenderSystem(WindowSystemBase* window);
		~RenderSystem();
		void Tick();
		void InitUIPass(UIBase* ui);
	};
}