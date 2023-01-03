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
		auto maxFrame = rhi->GetMaxFramesInFlight();
		auto swapchainExtent = rhi->GetSwapchainExtent();
		m_SwapchianFramebuffers.resize(maxFrame);
		for(uint8_t i=0; i< maxFrame; ++i) {
			m_SwapchianFramebuffers[i] = rhi->CreateFrameBuffer(m_MainPass, { rhi->GetSwapchainImageViews(i) }, swapchainExtent.width, swapchainExtent.height, 1);
		}

	}
	RenderSystem::~RenderSystem()
	{
		RHI::RHIInstance* rhi = RHI::GetInstance();
		// wait cmds submit
		rhi->QueueWaitIdle(rhi->GetGraphicsQueue());
		m_UIRenderer.Release();
		for(uint8_t i=0; i<rhi->GetMaxFramesInFlight(); ++i) {
			rhi->DestoryFramebuffer(m_SwapchianFramebuffers[i]);
		}
		m_SwapchianFramebuffers.clear();

		rhi->DestroyRenderPass(m_MainPass);

		rhi->Release();
	}

	void RenderSystem::Tick(){
		// begin render pass;
		GET_RHI(rhi);
		rhi->PrepareRendering();
		RHI::RCommandBuffer* cmd = rhi->GetCurrentCommandBuffer();
		rhi->BeginCommandBuffer(cmd, 0);
		TVector<RHI::RSClearValue> clearValues(1);
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		// todo depth
		//TVector<RHI::RSClearValue> clearValues(2);
		//clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		//clearValues[1].depthStencil = { 1.0f, 0 };
		rhi->CmdBeginRenderPass(cmd, m_MainPass, m_SwapchianFramebuffers[m_CurrentFrameIndex], 
			{ {0, 0}, rhi->GetSwapchainExtent() }, clearValues.size(), clearValues.data());

		m_UIRenderer.Tick();

		rhi->CmdEndRenderPass(cmd);
		rhi->EndCommandBuffer(cmd);
		rhi->QueueSubmitRendering(cmd);

		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

	}
	void RenderSystem::InitUIPass(UIBase* ui)
	{
		m_UIRenderer.Initialize(ui, m_MainPass);
	}
}
