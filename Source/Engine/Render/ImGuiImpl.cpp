#include "ImGuiImpl.h"
#include "Resource/Config/Config.h"
#include "RHI/RHI.h"
#include "RHI/RHIVulkan/RHIVulkan.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "Core/macro.h"

namespace Engine {

#define BRANCH_VULKAN \
	if(RHI_Vulkan == GetConfig()->GetRHIType()) {

#define BRANCH_END return ;}

	void ImGuiInitialize(RHI::RRenderPass* pass, uint32_t subpass)
	{
		BRANCH_VULKAN
		RHI::RHIVulkan* vkRHI = reinterpret_cast<RHI::RHIVulkan*>(RHI::GetInstance());
		RHI::RSVkImGuiInitInfo initInfo = vkRHI->GetImGuiInitInfo();
		ImGui_ImplGlfw_InitForVulkan(reinterpret_cast<GLFWwindow*>(initInfo.windowHandle), true);
		ImGui_ImplVulkan_InitInfo imGuiInit = {};
		imGuiInit.Instance = initInfo.instance;
		imGuiInit.PhysicalDevice = initInfo.physicalDevice;
		imGuiInit.Device = initInfo.device;
		imGuiInit.QueueFamily = initInfo.queueIndex;
		imGuiInit.Queue = initInfo.queue;
		imGuiInit.DescriptorPool = initInfo.descriptorPool;
		imGuiInit.Subpass = subpass;
		// may be different from the real swapchain image count
		// see ImGui_ImplVulkanH_GetMinImageCountFromPresentMode
		imGuiInit.MinImageCount = 3;
		imGuiInit.ImageCount = 3;
		ASSERT(ImGui_ImplVulkan_Init(&imGuiInit, reinterpret_cast<RHI::RRenderPassVk*>(pass)->handle), "Failed to init imgui!");
		BRANCH_END
	}
	void ImGuiRelease()
	{
		BRANCH_VULKAN
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		BRANCH_END
	}
	void ImGuiRenderDrawData(RHI::RCommandBuffer* cmd) {
		ImGui::Render();
		BRANCH_VULKAN ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), reinterpret_cast<RHI::RCommandBufferVk*>(cmd)->handle); BRANCH_END
	}
	void ImGuiCreateFontsTexture(RHI::RCommandBuffer* cmd)
	{
		BRANCH_VULKAN ASSERT(ImGui_ImplVulkan_CreateFontsTexture(reinterpret_cast<RHI::RCommandBufferVk*>(cmd)->handle), "Failed to upload imgui fonts!"); BRANCH_END
	}
	void ImGuiNewFrame() {
		BRANCH_VULKAN
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();
		BRANCH_END
	}
	void ImGuiDestroyFontUploadObjects()
	{
		BRANCH_VULKAN ImGui_ImplVulkan_DestroyFontUploadObjects(); BRANCH_END
	}
}
