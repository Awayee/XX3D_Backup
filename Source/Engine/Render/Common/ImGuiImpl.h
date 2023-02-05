#pragma once
#include "RHI/RHI.h"
#include <imgui.h>

namespace Engine {
	void ImGuiInitialize(RHI::RRenderPass* pass, uint32 subpass);
	void ImGuiRelease();
	void ImGuiNewFrame();
	void ImGuiRenderDrawData(RHI::RCommandBuffer* cmd);
	void ImGuiCreateFontsTexture(RHI::RCommandBuffer* cmd);
	void ImGuiDestroyFontUploadObjects();
}