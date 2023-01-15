#include "RenderSystem.h"
#include "ImGuiImpl.h"
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
		CreateRenderPasses();
		CreateSwapchainFramebuffers();
		CreateCommandBuffers();

		// register window func
		window->RegisterOnWindowSizeFunc([this](int w, int h) {
			this->OnWindowSizeChanged((uint32_t)w, (uint32_t)h);
		});

	}
	RenderSystem::~RenderSystem()
	{
		GET_RHI(rhi);
		// wait cmds submit
		rhi->QueueWaitIdle(rhi->GetGraphicsQueue());

		ImGuiRelease();

		for(auto& framebuffer: m_SwapchianFramebuffers) {
			rhi->DestoryFramebuffer(framebuffer);
		}
		m_SwapchianFramebuffers.clear();
		for(auto& cmd: m_CommandBuffers) {
			rhi->FreeCommandBuffer(cmd);
		}
		rhi->DestroyRenderPass(m_MainPass);

		rhi->Release();
	}

	void RenderSystem::Tick(){
		// window is hided, pause rendering
		if(!m_WindowAvailable) {
			return;
		}

		GET_RHI(rhi);
		// begin render pass;
		int swapchainImageIndex = rhi->PreparePresent(m_CurrentFrameIndex);
		if(-1 == swapchainImageIndex) {
			OnWindowSizeChanged(0, 0);
			return;
		}
		RHI::RCommandBuffer* cmd = m_CommandBuffers[m_CurrentFrameIndex];
		rhi->BeginCommandBuffer(cmd, 0);

		// todo depth
		RHI::RSClear clearValue;
		clearValue.clearValue = { 0.0f, 0.0f, 0.0f, 0.0f };
		//RHI::RSClear clearValues[2];
		//clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
		//clearValues[1].depthStencil = { 1.0f, 0 };
		rhi->CmdBeginRenderPass(cmd, m_MainPass, m_SwapchianFramebuffers[swapchainImageIndex],
			{ {0, 0}, rhi->GetSwapchainExtent() }, 1, &clearValue);

		if(nullptr != m_UIContent) {
			Engine::ImGuiNewFrame();
			m_UIContent->Tick();
			ImGui::Render();
			Engine::ImGuiRenderDrawData(ImGui::GetDrawData(), cmd);
		}

		rhi->CmdEndRenderPass(cmd);
		rhi->EndCommandBuffer(cmd);
		int res = rhi->QueueSubmitPresent(cmd, m_CurrentFrameIndex);
		if(-1 == res) {
			OnWindowSizeChanged(0, 0);
		}
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

	}
	void RenderSystem::InitUIPass(UIBase* ui)
	{
		m_UIContent = ui;
		Engine::ImGuiInitialize(m_MainPass, 0);
		GET_RHI(rhi);
		// upload font
		rhi->ImmediateCommit([](RHI::RCommandBuffer* cmd) {
			ImGuiCreateFontsTexture(cmd);
			});
		ImGuiDestroyFontUploadObjects();
	}

	void RenderSystem::CreateRenderPasses()
	{
		GET_RHI(rhi);
		// create render pass
		RHI::RSAttachment attachment{};
		attachment.format = rhi->GetSwapchainImageFormat();
		attachment.initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = RHI::IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachment.type = RHI::ATTACHMENT_COLOR;

		m_MainPass = rhi->CreateRenderPass(1, &attachment);
	}

	void RenderSystem::CreateSwapchainFramebuffers()
	{
		GET_RHI(rhi);
		// create swapchain framebuffers
		auto imageCount = rhi->GetSwapchainMaxImageCount();
		auto swapchainExtent = rhi->GetSwapchainExtent();
		m_SwapchianFramebuffers.resize(imageCount);
		for (uint8_t i = 0; i < imageCount; ++i) {
			RHI::RImageView* imageView = rhi->GetSwapchainImageView(i);
			m_SwapchianFramebuffers[i] = rhi->CreateFrameBuffer(m_MainPass, 1, &imageView, swapchainExtent.width, swapchainExtent.height, 1);
		}
	}

	void RenderSystem::CreateCommandBuffers()
	{
		GET_RHI(rhi);
		// create command buffers
		m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		for (uint8_t i = 0; i < m_CommandBuffers.size(); ++i) {
			m_CommandBuffers[i] = rhi->AllocateCommandBuffer(RHI::COMMAND_BUFFER_LEVEL_PRIMARY);
		}
	}

	void RenderSystem::OnWindowSizeChanged(uint32_t w, uint32_t h)
	{
		m_WindowAvailable = (0 != w && 0 != h);
		if(!m_WindowAvailable) {
			return;
		}

		// recreate resources
		GET_RHI(rhi);
		rhi->ResizeSwapchain(w, h);
		for (auto* cmd : m_CommandBuffers) {
			rhi->FreeCommandBuffer(cmd);
		}
		for (auto* fb : m_SwapchianFramebuffers) {
			rhi->DestoryFramebuffer(fb);
		}
		rhi->DestroyRenderPass(m_MainPass);
		CreateRenderPasses();
		CreateSwapchainFramebuffers();
		CreateCommandBuffers();
	}
}
