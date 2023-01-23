#include "RenderSystem.h"

#include <vulkan/vulkan_core.h>

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
		CreateImages();
		CreateRenderPasses();
		CreateFramebuffers();
		CreateCommandBuffers();

		// register window func
		window->RegisterOnWindowSizeFunc([this](int w, int h) {
			this->OnWindowSizeChanged((uint32)w, (uint32)h);
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
		m_Images.clear();

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

		rhi->CmdBeginRenderPass(cmd, m_Passes[PASS_MAIN], m_SwapchianFramebuffers[swapchainImageIndex], { {0, 0}, rhi->GetSwapchainExtent() });

		if(RENDER_DEFERRED == GetConfig()->GetRenderPath()) {
			m_MainScene->RenderGBuffer(cmd);
			rhi->CmdNextSubpass(cmd);
			m_MainScene->RenderLight(cmd);
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
		Engine::ImGuiInitialize(m_Passes[PASS_MAIN], 1);
		GET_RHI(rhi);
		// upload font
		rhi->ImmediateCommit([](RHI::RCommandBuffer* cmd) {
			ImGuiCreateFontsTexture(cmd);
			});
		ImGuiDestroyFontUploadObjects();
	}

	void RenderSystem::CreateImages()
	{
		m_Images.resize(ATTACHMENT_COUNT);
		GET_RHI(rhi);
		m_Images[ATTACHMENT_DEPTH].reset(new Texture2D(rhi->GetDepthFormat(), rhi->GetSwapchainExtent().width, rhi->GetSwapchainExtent().height,
			RHI::IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			RHI::IMAGE_ASPECT_DEPTH_BIT, nullptr));
		m_Images[ATTACHMENT_NORMAL].reset(new Texture2D(RHI::FORMAT_R8G8B8A8_UNORM, rhi->GetSwapchainExtent().width, rhi->GetSwapchainExtent().height,
			RHI::IMAGE_USAGE_SAMPLED_BIT | RHI::IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			RHI::IMAGE_ASPECT_COLOR_BIT, nullptr));
		m_Images[ATTACHMENT_ALBEDO].reset(new Texture2D(RHI::FORMAT_R8G8B8A8_UNORM, rhi->GetSwapchainExtent().width, rhi->GetSwapchainExtent().height,
			RHI::IMAGE_USAGE_SAMPLED_BIT | RHI::IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			RHI::IMAGE_ASPECT_COLOR_BIT,
			nullptr));
	}

	void RenderSystem::CreateRenderPasses()
	{
		m_Passes.resize(PASS_COUNT);

		GET_RHI(rhi);

		/*
		TVector<RHI::RSubpass> subpasses;
		// g buffer pass
		RHI::RSubpass gPass;
		gPass.Type = RHI::PIPELINE_GRAPHICS;
		gPass.ColorAttachments.resize(2);
		//	g buffer normal + roughness
		gPass.ColorAttachments[0].format = RHI::FORMAT_R8G8B8A8_UNORM;
		gPass.ColorAttachments[0].initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		gPass.ColorAttachments[0].finalLayout = RHI::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		gPass.ColorAttachments[0].refLayout = RHI::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//	g buffer color + metallic
		gPass.ColorAttachments[1] = gPass.ColorAttachments[0];
		gPass.DepthStencilAttachments.resize(1);
		gPass.DepthStencilAttachments[0].format = rhi->GetDepthFormat();
		gPass.DepthStencilAttachments[0].initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		gPass.DepthStencilAttachments[0].finalLayout = RHI::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		gPass.DepthStencilAttachments[0].refLayout = RHI::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		subpasses.push_back(std::move(gPass));

		// main subpass
		RHI::RSubpass lightPass;
		lightPass.Type = RHI::PIPELINE_GRAPHICS;
		RHI::RSAttachment atm;
		atm.format = rhi->GetSwapchainImageFormat();
		atm.initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		atm.finalLayout = RHI::IMAGE_LAYOUT_PRESENT_SRC_KHR;
		atm.refLayout = RHI::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		lightPass.ColorAttachments.push_back(atm);
		subpasses.push_back(lightPass);

		// dependencies
		TVector<RHI::RSubpassDependency> dependencies(2);
		dependencies[0].SrcSubPass = SUBPASS_INTERNAL;
		dependencies[0].DstSubPass = 0;
		dependencies[0].SrcStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].DstStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].SrcAccess = 0;
		dependencies[0].DstAccess = RHI::ACCESS_COLOR_ATTACHMENT_WRITE_BIT | RHI::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		// lighting pass depends on g buffer pass
		dependencies[1].SrcSubPass = 0;
		dependencies[1].DstSubPass = 1;
		dependencies[1].SrcStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[1].DstStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[1].SrcAccess = 0;
		dependencies[1].DstAccess = RHI::ACCESS_COLOR_ATTACHMENT_WRITE_BIT | RHI::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		m_Passes[PASS_MAIN] = rhi->CreateRenderPass(subpasses.size(), subpasses.data(), dependencies.size(), dependencies.data());
		*/
		TVector<RHI::RSAttachment> attachments(m_Images.size() + 1);
		// depth 
		attachments[ATTACHMENT_DEPTH].format = rhi->GetDepthFormat();
		attachments[ATTACHMENT_DEPTH].initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachments[ATTACHMENT_DEPTH].finalLayout = RHI::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[ATTACHMENT_DEPTH].refLayout = RHI::IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[ATTACHMENT_DEPTH].clear = { 1.0, 0 };
		// g buffer normal
		attachments[ATTACHMENT_NORMAL].format = RHI::FORMAT_R8G8B8A8_UNORM;
		attachments[ATTACHMENT_NORMAL].initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachments[ATTACHMENT_NORMAL].finalLayout = RHI::IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		attachments[ATTACHMENT_NORMAL].refLayout = RHI::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachments[ATTACHMENT_NORMAL].clear = { 0.0f, 0.0f, 0.0f, 0.0f };
		// g buffer albedo
		attachments[ATTACHMENT_ALBEDO] = attachments[ATTACHMENT_NORMAL];
		// swapchain
		RHI::RSAttachment& atmPresent = attachments.back();
		atmPresent.format = rhi->GetSwapchainImageFormat();
		atmPresent.initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		atmPresent.finalLayout = RHI::IMAGE_LAYOUT_PRESENT_SRC_KHR;
		atmPresent.refLayout = RHI::IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		atmPresent.clear = { 0.0f, 0.0f, 0.0f, 0.0f };

		TVector<RHI::RSubPassInfo> subpasses(2);
		subpasses[0].ColorAttachments = { ATTACHMENT_NORMAL, ATTACHMENT_ALBEDO };
		subpasses[0].DepthStencilAttachment = ATTACHMENT_DEPTH;
		subpasses[1].ColorAttachments = { (uint32)attachments.size() - 1 };
		subpasses[1].DepthStencilAttachment = VK_NULL_HANDLE;

		// dependencies
		TVector<RHI::RSubpassDependency> dependencies(2);
		dependencies[0].SrcSubPass = SUBPASS_INTERNAL;
		dependencies[0].DstSubPass = 0;
		dependencies[0].SrcStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].DstStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].SrcAccess = 0;
		dependencies[0].DstAccess = RHI::ACCESS_COLOR_ATTACHMENT_WRITE_BIT | RHI::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		// lighting pass depends on g buffer pass
		dependencies[1].SrcSubPass = 0;
		dependencies[1].DstSubPass = 1;
		dependencies[1].SrcStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[1].DstStage = RHI::PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | RHI::PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[1].SrcAccess = 0;
		dependencies[1].DstAccess = RHI::ACCESS_COLOR_ATTACHMENT_WRITE_BIT | RHI::ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		m_Passes[PASS_MAIN] = rhi->CreateRenderPass(attachments.size(), attachments.data(),
			subpasses.size(), subpasses.data(),
			dependencies.size(), dependencies.data());
	}

	void RenderSystem::CreateFramebuffers()
	{
		GET_RHI(rhi);
		// create swapchain framebuffers
		auto imageCount = rhi->GetSwapchainMaxImageCount();
		auto swapchainExtent = rhi->GetSwapchainExtent();
		m_SwapchianFramebuffers.resize(imageCount);
		for (uint8_t i = 0; i < imageCount; ++i) {
			TVector<RHI::RImageView*> attachments;
			for(auto& img: m_Images) {
				attachments.push_back(img->GetImageView());
			}
			attachments.push_back(rhi->GetSwapchainImageView(i));
			m_SwapchianFramebuffers[i] = rhi->CreateFrameBuffer(m_Passes[PASS_MAIN], attachments.size(), attachments.data(), swapchainExtent.width, swapchainExtent.height, 1);
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
		for (auto fb : m_SwapchianFramebuffers) {
			if(fb)rhi->DestroyFramebuffer(fb);
		}
		for(auto pass: m_Passes) {
			if(pass) rhi->DestroyRenderPass(pass);
		}
		CreateImages();
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
