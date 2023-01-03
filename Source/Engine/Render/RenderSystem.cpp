#include "RenderSystem.h"
#include "Engine/Window/WindowSystem.h"

namespace Engine {
	RenderSystem::RenderSystem(WindowSystemBase* window)
	{
		// init rhi
		RHI::RSInitInfo initInfo;
		initInfo.applicationName = "xxEngine";
		initInfo.enableDebug = true;
		initInfo.enableGeometryShader = true;
		window->GetWindowSize(&initInfo.windowWidth, &initInfo.windowHeight);
		initInfo.windowHandle = window->GetWindowHandle();

		RHI::RHIInstance* rhi = RHI::GetInstance();
		rhi->Initialize(&initInfo);

		// create render pass
		RHI::RSAttachment attachment{};
		attachment.format = RHI::FORMAT_R8G8B8A8_SNORM;
		attachment.initialLayout = RHI::IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = RHI::IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachment.type = RHI::ATTACHMENT_COLOR;

		rhi->CreateRenderPass(&m_MainPass, 1, &attachment);
	}
	RenderSystem::~RenderSystem()
	{
		m_UIRenderer.Release();
		RHI::RHIInstance* rhi = RHI::GetInstance();
		rhi->DestroyRenderPass(&m_MainPass);
		rhi->Release();
	}

	void RenderSystem::Tick(){
		m_UIRenderer.Tick();
	}
	void RenderSystem::InitUIPass(UIBase* ui)
	{
		m_UIRenderer.Initialize(ui, &m_MainPass);
	}
}
