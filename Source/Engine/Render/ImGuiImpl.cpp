#include "ImGuiImpl.h"
#include "Resource/Config/Config.h"
#include "RHI/RHIInstance.h"
#include "RHI/RHIVulkan/RHIVulkan.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include "Core/macro.h"

namespace Engine {

#define RHI_VULKAN_BEGIN \
	if(Resource::RHI_Vulkan == Resource::GetConfigManager()->GetRHIType()) {

#define BRANCH_END return ;}

	void ImGuiInitialize(RHI::RRenderPass* pass, uint32_t subpass)
	{
		RHI_VULKAN_BEGIN
		GET_RHI(rhi);
		RHI::RVkRenderPass* vkPass = reinterpret_cast<RHI::RVkRenderPass*>(pass);
		RHI::RHIVulkan* vkRHI = reinterpret_cast<RHI::RHIVulkan*>(rhi);
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
		ASSERT(ImGui_ImplVulkan_Init(&imGuiInit, reinterpret_cast<RHI::RVkRenderPass*>(pass)->m_VkRenderPass), "Failed to init imgui!");
		BRANCH_END
	}
	void ImGuiRelease()
	{
		RHI_VULKAN_BEGIN ImGui_ImplVulkan_Shutdown(); BRANCH_END
	}
	void ImGuiRenderDrawData(ImDrawData* data, RHI::RCommandBuffer* cmd) {
		RHI_VULKAN_BEGIN ImGui_ImplVulkan_RenderDrawData(data, reinterpret_cast<RHI::RVkCommandBuffer*>(cmd)->m_VkCmd); BRANCH_END
	}
	void ImGuiCreateFontsTexture(RHI::RCommandBuffer* cmd)
	{
		RHI_VULKAN_BEGIN ImGui_ImplVulkan_CreateFontsTexture(reinterpret_cast<RHI::RVkCommandBuffer*>(cmd)->m_VkCmd); BRANCH_END
	}
	void ImGuiNewFrame() {
		RHI_VULKAN_BEGIN
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();
		BRANCH_END
	}
	void ImGuiDestroyFontUploadObjects()
	{
		RHI_VULKAN_BEGIN ImGui_ImplVulkan_DestroyFontUploadObjects(); BRANCH_END
	}
}
