#include "RenderSystem.h"

#include <vulkan/vulkan_core.h>

#include "RenderMacro.h"
#include "ImGuiImpl.h"
#include "../Window/WindowSystem.h"
#include "../Scene/RenderScene.h"
#include "Resource/Config/Config.h"

namespace Engine {
	RenderSystem::RenderSystem(WindowSystemBase* window)
	{
		int windowWidth, windowHeight;
		window->GetWindowSize(&windowWidth, &windowHeight);
		// init rhi
		{
			RHI::RSInitInfo initInfo;
			initInfo.applicationName = PROJECT_NAME;
			initInfo.enableDebug = true;
			initInfo.enableGeometryShader = true;
			initInfo.windowWidth = windowWidth;
			initInfo.windowHeight = windowHeight;
			initInfo.windowHandle = window->GetWindowHandle();
			initInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
			RHI::GetInstance()->Initialize(&initInfo);
		}

		DescsMgr::Initialize();
		m_PresentPass.reset(new PresentPass());
		m_GBufferPipeline.reset(new GBufferPipeline(m_PresentPass.get(), PresentPass::SUBPASS_GBUFFER));
		CreateCommandBuffers();
		// register window func
		window->RegisterOnWindowSizeFunc([this](int w, int h) {
			this->OnWindowSizeChanged((uint32)w, (uint32)h);
		});

	}
	RenderSystem::~RenderSystem()
	{
		ImGuiRelease();
		RenderScene::Clear();
		DescsMgr::Release();
		m_GBufferPipeline.reset();
		m_PresentPass.reset();

		GET_RHI(rhi);
		for(auto& cmd: m_CommandBuffers) {
			rhi->FreeCommandBuffer(cmd);
		}

		rhi->Release();
	}

	void RenderSystem::SetEnable(bool enable)
	{
		m_Enable = enable;
		if(!enable) {
			RHI_INSTANCE->WaitGraphicsQueue();
		}
	}

	void RenderSystem::Tick(){
		if(!m_Enable) {
			return;
		}

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

		m_PresentPass->SetImageIndex(swapchainImageIndex);
		m_PresentPass->Begin(cmd);

		auto mainScene = RenderScene::GetDefaultScene();
		if(RENDER_DEFERRED == GetConfig()->GetRenderPath()) {
			m_GBufferPipeline->Bind(cmd);
			mainScene->RenderGBuffer(cmd, m_GBufferPipeline->GetLayout());
			rhi->CmdNextSubpass(cmd);
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
		// the last subpass is for ui
		Engine::ImGuiInitialize(m_PresentPass->GetRHIPass(), 1);
		GET_RHI(rhi);
		// upload font
		rhi->ImmediateCommit([](RHI::RCommandBuffer* cmd) {
			ImGuiCreateFontsTexture(cmd);
			});
		ImGuiDestroyFontUploadObjects();
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

	void RenderSystem::OnWindowSizeChanged(uint32 w, uint32 h)
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
		m_PresentPass.reset(new PresentPass());
		m_GBufferPipeline.reset(new GBufferPipeline(m_PresentPass.get(), PresentPass::SUBPASS_GBUFFER));
		CreateCommandBuffers();
	}
}
