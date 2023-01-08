#pragma once
#include "UIBase.h"
#include "RHI/RHIInstance.h"
#include "UIRenderer.h"

namespace Engine {
	class WindowSystemBase;

	class RenderSystem {
	private:
		UIRenderer m_UIRenderer;
		//RenderPasses
		RHI::RRenderPass* m_MainPass{nullptr};
		TVector<RHI::RCommandBuffer*> m_CommandBuffers;
		TVector<RHI::RFramebuffer*> m_SwapchianFramebuffers;
		uint8_t m_CurrentFrameIndex{0};
		bool m_WindowAvaliable{ true };
		bool m_WindowSizeChanged{ false };

	public:
		RenderSystem() = default;
		RenderSystem(WindowSystemBase* window);
		~RenderSystem();
		void Tick();
		void InitUIPass(UIBase* ui);

	private:
		void CreateRenderPasses();
		void CreateSwapchainFramebuffers();
		void CreateCommandBuffers();
		void OnWindowSizeChanged(uint32_t w, uint32_t h);
	};
}