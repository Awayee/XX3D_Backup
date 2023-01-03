#include "UIRenderer.h"
#include "ImGuiImpl.h"
#include "RHI/RHIInstance.h"


namespace Engine {
	void UIRenderer::Initialize(UIBase* ui, RHI::RRenderPass* pass)
	{
		m_UIContent = ui;
		m_RenderPass = pass;
		Engine::ImGuiInitialize(m_RenderPass, 0);
		GET_RHI(rhi);
		// upload font
		RHI::RCommandBuffer* cmd = rhi->CreateCommandBuffer(RHI::COMMAND_BUFFER_LEVEL_PRIMARY);
		rhi->BeginCommandBuffer(cmd, RHI::COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		ImGuiCreateFontsTexture(cmd);
		rhi->EndCommandBuffer(cmd);
		rhi->QueueSubmit(rhi->GetGraphicsQueue(), { cmd }, {}, {}, nullptr);
		rhi->QueueWaitIdle(rhi->GetGraphicsQueue());
		rhi->FreeCommandBuffer(cmd);
		ImGuiDestroyFontUploadObjects();
	}
	void UIRenderer::Tick()
	{
		if(nullptr == m_UIContent) {
			return;
		}
		// Start the Dear ImGui frame
		Engine::ImGuiNewFrame();
		m_UIContent->Tick();
		ImGui::Render();
		// todo draw
		//RHI::RCommandBuffer* cmd = RHI::GetInstance()->GetCurrentCommandBuffer();
		//Engine::ImGuiRenderDrawData(ImGui::GetDrawData(), cmd);
	}
	void UIRenderer::Release()
	{
		Engine::ImGuiRelease();
	}
}