#pragma once
#include "RHI/RHI.h"
#include <imgui.h>

namespace Engine {
	void ImGuiInitialize(RHI::RRenderPass* pass, uint32_t subpass);
	void ImGuiRelease();
	void ImGuiNewFrame();
	void ImGuiRenderDrawData(ImDrawData* data, RHI::RCommandBuffer* cmd);
	void ImGuiCreateFontsTexture(RHI::RCommandBuffer* cmd);
	void ImGuiDestroyFontUploadObjects();
}