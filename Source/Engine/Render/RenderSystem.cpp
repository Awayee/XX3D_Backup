#include "RenderSystem.h"
#include "ImGuiImpl.h"
#include "Engine/Window/WindowSystem.h"
#include "RenderMacro.h"
#include "Resource/Config/Config.h"

namespace Engine {
	RenderSystem::RenderSystem(WindowSystemBase* window)
	{
		// init rhi
		{
			RHI::RSInitInfo initInfo;
			initInfo.applicationName = PROJECT_NAME;
			initInfo.enableDebug = true;
			initInfo.enableGeometryShader = true;
			window->GetWindowSize(&initInfo.windowWidth, &initInfo.windowHeight);
			initInfo.windowHandle = window->GetWindowHandle();
			initInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
			RHI::GetInstance()->Initialize(&initInfo);
		}
		CreateDepthImage();
		CreateRenderPasses();
		CreateFramebuffers();
		CreateCommandBuffers();

		// register window func
		window->RegisterOnWindowSizeFunc([this](int w, int h) {
			this->OnWindowSizeChanged((uint32_t)w, (uint32_t)h);
		});

	}
	RenderSystem::~RenderSystem()
	{
		GET_RHI(rhi);
		rhi->QueueWaitIdle(rhi->GetGraphicsQueue());

		ImGuiRelease();

		for(auto& framebuffer: m_SwapchianFramebuffers) {
			rhi->DestroyFramebuffer(framebuffer);
		}
		m_SwapchianFramebuffers.clear();
		for(auto& cmd: m_CommandBuffers) {
			rhi->FreeCommandBuffer(cmd);
		}
		m_DepthImage.reset();

		for(auto pass: m_Passes) {
			if(pass) rhi->DestroyRenderPass(pass);
		}
		m_Passes.clear();

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

		// render scene forward
		RHI::RSClear clearValues[2];
		clearValues[0].clearType = RHI::CLEAR_VALUE_COLOR;
		clearValues[0].clearValue.color = { 0.0f, 0.0f, 0.0f, 0.0f };
		clearValues[1].clearType = RHI::CLEAR_VALUE_DEPTH_STENCIL;
		clearValues[1].clearValue.depthStencil = { 1.0f, 0 };
		rhi->CmdBeginRenderPass(cmd, m_Passes[PASS_MAIN], m_SwapchianFramebuffers[swapchainImageIndex],
			{ {0, 0}, rhi->GetSwapchainExtent() }, 2, clearValues);

		if(RENDER_DEFERRED == GetConfig()->GetRenderPath()) {
			RenderSceneDeferred(cmd);
		}
		else if (RENDER_FORWARD == GetConfig()->GetRenderPath()) {
			RenderSceneForward(cmd);
		}

		// render ui
		if(nullptr != m_UIContent) {
			Engine::ImGuiNewFrame();
			m_UIContent->Tick();
			Engine::ImGuiRenderDrawData(cmd);
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
		Engine::ImGuiInitialize(m_Passes[PASS_MAIN], 0);
		GET_RHI(rhi);
		// upload font
		rhi->ImmediateCommit([](RHI::RCommandBuffer* cmd) {
			ImGuiCreateFontsTexture(cmd);
			});
		ImGuiDestroyFontUploadObjects();
	}

	void RenderSystem::CreateDepthImage()
	{
		GET_RHI(rhi);
		m_DepthImage.reset(new Texture2D(rhi->GetDepthFormat(), rhi->GetSwapchainExtent().width, rhi->GetSwapchainExtent().height,
			RHI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, nullptr));
	}

	void RenderSystem::CreateRenderPasses()
	{
		GET_RHI(rhi);

		// create render pass
		RHI::RSAttachment attachment[2];
		attachment[0].format = rhi->GetSwapchainImageFormat();
		attachment[0].initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachment[0].finalLayout = RHI::IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachment[0].type = RHI::ATTACHMENT_COLOR;
		attachment[1].format = rhi->GetDepthFormat();
		attachment[1].initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachment[1].finalLayout = RHI::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachment[1].type = RHI::ATTACHMENT_DEPTH;

		m_Passes.resize(PASS_COUNT);
		m_Passes[PASS_MAIN] = rhi->CreateRenderPass(2, attachment);
	}

	void RenderSystem::CreateFramebuffers()
	{
		GET_RHI(rhi);
		// create swapchain framebuffers
		auto imageCount = rhi->GetSwapchainMaxImageCount();
		auto swapchainExtent = rhi->GetSwapchainExtent();
		m_SwapchianFramebuffers.resize(imageCount);
		for (uint8_t i = 0; i < imageCount; ++i) {
			RHI::RImageView* imageVies[2] = { rhi->GetSwapchainImageView(i), m_DepthImage->GetImageView() };
			m_SwapchianFramebuffers[i] = rhi->CreateFrameBuffer(m_Passes[PASS_MAIN], 2, imageVies, swapchainExtent.width, swapchainExtent.height, 1);
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
		for (auto cmd : m_CommandBuffers) {
			if(cmd)rhi->FreeCommandBuffer(cmd);
		}
		for (auto fb : m_SwapchianFramebuffers) {
			if(fb)rhi->DestroyFramebuffer(fb);
		}
		for(auto pass: m_Passes) {
			if(pass) rhi->DestroyRenderPass(pass);
		}
		CreateDepthImage();
		CreateRenderPasses();
		CreateFramebuffers();
		CreateCommandBuffers();
	}
	void RenderSystem::RenderSceneForward(RHI::RCommandBuffer* cmd)
	{
	}
	void RenderSystem::RenderSceneDeferred(RHI::RCommandBuffer* cmd)
	{
	}
}
