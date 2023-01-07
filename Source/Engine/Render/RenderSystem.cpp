#include "RenderSystem.h"
#include "Engine/Window/WindowSystem.h"
#include "RenderMacro.h"

namespace Engine {
	RenderSystem::RenderSystem(WindowSystemBase* window)
	{
		// init rhi
		{
			RHI::RSInitInfo initInfo;
			initInfo.applicationName = "xxEngine";
			initInfo.enableDebug = true;
			initInfo.enableGeometryShader = true;
			window->GetWindowSize(&initInfo.windowWidth, &initInfo.windowHeight);
			initInfo.windowHandle = window->GetWindowHandle();
			initInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
			RHI::GetInstance()->Initialize(&initInfo);
		}

		GET_RHI(rhi);

		// create render pass
		RHI::RSAttachment attachment{};
		attachment.format = rhi->GetSwapchainImageFormat();
		attachment.initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = RHI::IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachment.type = RHI::ATTACHMENT_COLOR;
		m_MainPass = rhi->CreateRenderPass(1, &attachment);

		// create swapchain framebuffers
		auto imageCount = rhi->GetSwapchainMaxImageCount();
		auto swapchainExtent = rhi->GetSwapchainExtent();
		m_SwapchianFramebuffers.resize(imageCount);
		for(uint8_t i=0; i< imageCount; ++i) {
			RHI::RImageView* imageView = rhi->GetSwapchainImageView(i);
			m_SwapchianFramebuffers[i] = rhi->CreateFrameBuffer(m_MainPass, 1, &imageView, swapchainExtent.width, swapchainExtent.height, 1);
		}

	}
	RenderSystem::~RenderSystem()
	{
		RHI::RHIInstance* rhi = RHI::GetInstance();
		// wait cmds submit
		rhi->QueueWaitIdle(rhi->GetGraphicsQueue());
		m_UIRenderer.Release();
		for(auto& framebuffer: m_SwapchianFramebuffers) {
			rhi->DestoryFramebuffer(framebuffer);
		}
		m_SwapchianFramebuffers.clear();

		rhi->DestroyRenderPass(m_MainPass);

		rhi->Release();
	}

	void RenderSystem::Tick(){
		// begin render pass;
		GET_RHI(rhi);
		uint32_t swapchainImageIndex = rhi->PrepareRendering(m_CurrentFrameIndex);
		RHI::RCommandBuffer* cmd = rhi->GetCurrentCommandBuffer(m_CurrentFrameIndex);
		rhi->BeginCommandBuffer(cmd, 0);

		// todo depth
		RHI::RSClear clearValue;
		clearValue.clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };
		//RHI::RSClear clearValues[2];
		//clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		//clearValues[1].depthStencil = { 1.0f, 0 };
		rhi->CmdBeginRenderPass(cmd, m_MainPass, m_SwapchianFramebuffers[swapchainImageIndex],
			{ {0, 0}, rhi->GetSwapchainExtent() }, 1, &clearValue);

		m_UIRenderer.Tick(cmd);

		rhi->CmdEndRenderPass(cmd);
		rhi->EndCommandBuffer(cmd);
		rhi->QueueSubmitRendering(cmd, m_CurrentFrameIndex);

		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

	}
	void RenderSystem::InitUIPass(UIBase* ui)
	{
		m_UIRenderer.Initialize(ui, m_MainPass);
	}
}
