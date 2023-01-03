#pragma once
#include <vk_mem_alloc.h>
#include "../RHI.h"
#include "RHIVKResources.h"
#include "Core/Container/Container.h"

namespace RHI{
	class RHIVulkan: public RHIInstance {
#pragma region private members
	private:
		uint8_t m_MaxFramesInFlight{ 3 };
		uint8_t m_CurrentFrame {0};
		bool m_EnableValidationLayers{ false };
		bool m_EnableDebugUtils{ false };
		bool m_EnableGeometryShader{ false };
		std::string m_AppName;
		const TVector<char const*> m_ValidationLayers{ "VK_LAYER_KHRONOS_validation" };
		TVector<char const*> m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		void* m_WindowHandle;

		bool m_Initialized{ false };

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
		VkSurfaceKHR m_Surface;
		VkPhysicalDevice m_PhysicalDevice;
		// physical device's info
		struct SPhyicalDeviceInfo {
			int score{ -1 };
			int graphicsIndex{ -1 };
			int presentIndex{ -1 };
			int computeIndex{ -1 };
			uint32_t imageCount{ 0 };
			VkSurfaceFormatKHR swapchainFormat{ VK_FORMAT_UNDEFINED };
			VkPresentModeKHR swapchainPresentMode{ VK_PRESENT_MODE_MAX_ENUM_KHR };
			VkSurfaceTransformFlagBitsKHR swapchainTransform{ VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR };
			VkExtent2D swapchainExtent{ 0, 0 };
		} m_PhysicalDeviceInfo;

		RQueueVk m_GraphicsQueue;
		RQueueVk m_ComputeQueue;
		RQueueVk m_PresentQueue;

		VkDevice m_Device{ nullptr };
		VkFormat m_DepthFormat;

		VkCommandPool m_RHICommandPool;
		TVector<VkCommandPool> m_CommandPools;
		TVector<RCommandBufferVk> m_CommandBuffers;
		VkDescriptorPool m_DescriptorPool;
		// sync
		TVector<VkSemaphore> m_ImageAvaliableSemaphores;
		TVector<VkSemaphore> m_PresentationFinishSemaphores;
		TVector<VkSemaphore> m_ImageAvaliableForTextureCopySemaphores;
		TVector<VkFence> m_IsFrameInFlightFences;
		VmaAllocator m_VMA{ nullptr };


		VkSwapchainKHR m_Swapchain{ nullptr };
		RSExtent2D m_SwapchainExtent;
		uint32_t m_CurrentSwapchainImageIndex;
		TVector<VkImage> m_SwapchainImages;
		TVector<RImageViewVk> m_SwapchainImageViews;
		VkRect2D m_Scissor;

		// function pointers
		PFN_vkCmdBeginDebugUtilsLabelEXT _vkCmdBeginDebugUtilsLabelEXT;
		PFN_vkCmdEndDebugUtilsLabelEXT   _vkCmdEndDebugUtilsLabelEXT;
		PFN_vkWaitForFences         _vkWaitForFences;
		PFN_vkResetFences           _vkResetFences;
		PFN_vkResetCommandPool      _vkResetCommandPool;
		PFN_vkBeginCommandBuffer    _vkBeginCommandBuffer;
		PFN_vkEndCommandBuffer      _vkEndCommandBuffer;
		PFN_vkCmdBeginRenderPass    _vkCmdBeginRenderPass;
		PFN_vkCmdNextSubpass        _vkCmdNextSubpass;
		PFN_vkCmdEndRenderPass      _vkCmdEndRenderPass;
		PFN_vkCmdBindPipeline       _vkCmdBindPipeline;
		PFN_vkCmdSetViewport        _vkCmdSetViewport;
		PFN_vkCmdSetScissor         _vkCmdSetScissor;
		PFN_vkCmdBindVertexBuffers  _vkCmdBindVertexBuffers;
		PFN_vkCmdBindIndexBuffer    _vkCmdBindIndexBuffer;
		PFN_vkCmdBindDescriptorSets _vkCmdBindDescriptorSets;
		PFN_vkCmdDrawIndexed        _vkCmdDrawIndexed;
		PFN_vkCmdClearAttachments   _vkCmdClearAttachments;

	private:
		bool CheckValidationLayerSupport();
		TVector<const char*> GetRequiredExtensions();
		void CreateInstance();
		void InitializeDebugMessenger();
		void CreateWindowSurface();
		SPhyicalDeviceInfo GetPhysicalDeviceInfo(VkPhysicalDevice physicalDevice);
		void InitializePhysicalDevice();
		void CreateLogicalDevice();
		VkFormat FindDepthFormat();
		void CreateCommandPools();
		void CreateCommandBuffers();
		void CreateDescriptorPool();
		void CreateSyncResources();
		void CreateMemoryAllocator();
		void CreateSwapchain();
		void ClearSwapchain();
#pragma endregion
	public:
		RHIVulkan() = default;
		void Initialize(const RSInitInfo* initInfo) override;
		void Release() override;
		RSVkImGuiInitInfo GetImGuiInitInfo();
		RCommandBuffer* GetCurrentCommandBuffer() override;
		uint8_t GetMaxFramesInFlight() override { return m_MaxFramesInFlight; }
		RFormat GetSwapchainImageFormat() override { return (RFormat)m_PhysicalDeviceInfo.swapchainFormat.format; }
		const RSExtent2D& GetSwapchainExtent() override { return m_SwapchainExtent; }
		RRenderPass* CreateRenderPass(uint32_t attachmentCount, RSAttachment* attachments) override;
		void DestroyRenderPass(RRenderPass* pass) override;
		RQueue* GetGraphicsQueue() override;
		void QueueSubmit(RQueue* queue,
			uint32_t cmdCount, RCommandBuffer* cmds,
			uint32_t waitSemaphoreCount, RSemaphore* waitSemaphores, RPipelineStageFlags* waitStageFlags,
			uint32_t signalSemaphoreCount, RSemaphore* signalSemaphores,
			RFence* fence) override;
		void QueueWaitIdle(RQueue* queue)override;
		RImageView* GetSwapchainImageViews(uint8_t i) override;
		RFramebuffer* CreateFrameBuffer(RRenderPass* pass, const TVector<RImageView*>& imageViews, uint32_t width, uint32_t height, uint32_t layers) override;
		void DestoryFramebuffer(RFramebuffer* framebuffer) override;
		RCommandBuffer* CreateCommandBuffer(RCommandBufferLevel level)override;
		void BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags) override;
		void EndCommandBuffer(RCommandBuffer* cmd) override;
		void FreeCommandBuffer(RCommandBuffer* cmd) override;
		void CmdBeginRenderPass(RCommandBuffer* cmd, RRenderPass* pass, RFramebuffer* framebuffer, RSRect2D renderArea, uint32_t clearValueCount, const RSClearValue* clearValues)override;
		void CmdEndRenderPass(RCommandBuffer* cmd) override;
		void ImmediateSubmit(CmdFunc func) override;
		void PrepareRendering() override;
		void QueueSubmitRendering(RCommandBuffer* cmd) override;
	};
}