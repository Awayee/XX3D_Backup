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
		rhi->ImmediateCommit([](RHI::RCommandBuffer* cmd) {
			ImGuiCreateFontsTexture(cmd);
		});
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
		RHI::RCommandBuffer* cmd = RHI::GetInstance()->GetCurrentCommandBuffer();
		Engine::ImGuiRenderDrawData(ImGui::GetDrawData(), cmd);
	}
	void UIRenderer::Release()
	{
		Engine::ImGuiRelease();
	}
}