#include "RenderSystem.h"

#include <vulkan/vulkan_core.h>

#include "RenderMacro.h"
#include "ImGuiImpl.h"
#include "../Window/WindowSystem.h"
#include "../Scene/RenderScene.h"
#include "Render/Material/Material.h"
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
		SamplerMgr::Initialize();
		CreateRenderResources();
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
		SamplerMgr::Release();
		MaterialMgr::Release();
		TextureMgr::Release();
		m_GBufferPipeline.reset();
		m_DeferredLightingPipeline.reset();
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
			// wait queue before disabled
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

		auto mainScene = RenderScene::GetDefaultScene();
		mainScene->Update();

		GET_RHI(rhi);
		// begin render pass;
		int swapchainImageIndex = rhi->PreparePresent(m_CurrentFrameIndex);
		if(-1 == swapchainImageIndex) {
			OnWindowSizeChanged(0, 0);
			return;
		}
		RHI::RCommandBuffer* cmd = m_CommandBuffers[m_CurrentFrameIndex];
		cmd->Begin(0);

		m_PresentPass->SetImageIndex(swapchainImageIndex);
		m_PresentPass->Begin(cmd);

		if(RENDER_DEFERRED == GetConfig()->GetRenderPath()) {
			m_GBufferPipeline->Bind(cmd);
			mainScene->RenderGBuffer(cmd, m_GBufferPipeline->GetLayout());
			cmd->NextSubpass();
			RenderDeferredLighting(cmd);
		}

		// render ui
		if(nullptr != m_UIContent) {
			Engine::ImGuiNewFrame();
			m_UIContent->Tick();
			Engine::ImGuiRenderDrawData(cmd);
		}

		cmd->EndRenderPass();
		cmd->End();
		int res = rhi->QueueSubmitPresent(cmd, m_CurrentFrameIndex);
		if(-1 == res) {
			OnWindowSizeChanged(0, 0);
		}
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

	}

	void RenderSystem::RenderDeferredLighting(RHI::RCommandBuffer* cmd)
	{
		m_DeferredLightingPipeline->Bind(cmd);
		cmd->BindDescriptorSet(m_DeferredLightingPipeline->GetLayout(), m_DeferredLightingDescs, 0, RHI::PIPELINE_GRAPHICS);
		cmd->Draw(6, 1, 0, 0);
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


	void RenderSystem::CreateRenderResources()
	{
		m_PresentPass.reset(new DeferredLightingPass());
		m_GBufferPipeline.reset(new GBufferPipeline(m_PresentPass.get(), DeferredLightingPass::SUBPASS_BASE));
		m_DeferredLightingPipeline.reset(new DeferredLightingPipeline(m_PresentPass.get(), DeferredLightingPass::SUBPASS_DEFERRED_LIGHTING));
		// create command buffers
		GET_RHI(rhi);
		m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
		for (uint8 i = 0; i < m_CommandBuffers.size(); ++i) {
			m_CommandBuffers[i] = rhi->AllocateCommandBuffer(RHI::COMMAND_BUFFER_LEVEL_PRIMARY);
		}
		// create descriptor set
		m_DeferredLightingDescs = rhi->AllocateDescriptorSet(DescsMgr::Get(DESCS_DEFERRED_LIGHTING));
		m_DeferredLightingDescs->UpdateUniformBuffer(0, RenderScene::GetDefaultScene()->m_CameraUniform.Buffer);
		m_DeferredLightingDescs->UpdateUniformBuffer(1, RenderScene::GetDefaultScene()->m_LightUniform.Buffer);
		m_DeferredLightingDescs->UpdateInputAttachment(2, m_PresentPass->GetAttachment(DeferredLightingPass::ATTACHMENT_NORMAL));
		m_DeferredLightingDescs->UpdateInputAttachment(3, m_PresentPass->GetAttachment(DeferredLightingPass::ATTACHMENT_ALBEDO));
		m_DeferredLightingDescs->UpdateInputAttachment(4, m_PresentPass->GetAttachment(DeferredLightingPass::ATTACHMENT_DEPTH));
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
		CreateRenderResources();
	}
}
