#pragma once
#include "UIBase.h"
#include "RHI/RHIInstance.h"
#include "Core/Container/Container.h"

namespace Engine {
	class WindowSystemBase;

	class RenderSystem {
	private:
		//RenderPasses
		RHI::RRenderPass* m_MainPass{nullptr};
		TVector<RHI::RCommandBuffer*> m_CommandBuffers;
		TVector<RHI::RFramebuffer*> m_SwapchianFramebuffers;
		RHI::RImage* m_DepthImage;
		RHI::RImageView* m_DepthImageView;
		RHI::RFramebuffer* m_DepthFramebuffer;

		uint8_t m_CurrentFrameIndex{0};
		bool m_WindowAvailable{ true };
		UIBase* m_UIContent;

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