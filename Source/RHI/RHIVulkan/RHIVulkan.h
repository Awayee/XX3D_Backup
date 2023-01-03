#pragma once
#include <vk_mem_alloc.h>
#include "../RHI.h"
#include "RHIVKResources.h"
#include "Core/Container/Container.h"

namespace RHI{

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void*);

	class RHIVulkan: public RHIInstance {
#pragma region private members
	private:
		static uint8_t const s_MaxFramesInFlight{ 3 };
		uint8_t m_CurrentFrame {0};
		bool m_IsNormal{ true };
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

		RVkQueue m_GraphicsQueue;
		RVkQueue m_ComputeQueue;
		RVkQueue m_PresentQueue;

		VkDevice m_Device{ nullptr };
		VkFormat m_DepthFormat;

		VkCommandPool m_RHICommandPool;
		VkCommandPool m_CommandPools[s_MaxFramesInFlight];
		RVkCommandBuffer m_CommandBuffers[s_MaxFramesInFlight];
		VkDescriptorPool m_DescriptorPool;
		// sync
		VkSemaphore m_ImageAvaliableSemaphores[s_MaxFramesInFlight];
		VkSemaphore m_PresentationFinishSemaphores[s_MaxFramesInFlight];
		VkSemaphore m_ImageAvaliableForTextureCopySemaphores[s_MaxFramesInFlight];
		VkFence m_IsFrameInFlightFences[s_MaxFramesInFlight];
		VmaAllocator m_VMA{ nullptr };


		VkSwapchainKHR m_Swapchain{ nullptr };
		TVector<VkImage> m_SwapchainImages;
		TVector<VkImageView> m_SwapchainImageViews;
		TVector<VkFramebuffer> m_SwapchainFramebuffers;
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
		friend VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void*);

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

		void CreateRenderPass(RRenderPass* pass, uint32_t attachmentCount, RSAttachment* attachments) override;
		void DestroyRenderPass(RRenderPass* pass) override;
		RCommandBuffer* CreateCommandBuffer(RCommandBufferLevel level)override;
		void BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags) override;
		void EndCommandBuffer(RCommandBuffer* cmd) override;
		void FreeCommandBuffer(RCommandBuffer*& cmd) override;
		RQueue* GetGraphicsQueue() override;
		void QueueSubmit(RQueue* queue, TVector<RCommandBuffer*> cmds, TVector<TPair<RSemaphore*, RPipelineStageFlags>> waitSemaphores, TVector<RSemaphore*> signalSemaphores, RFence* fence) override;
		void QueueWaitIdle(RQueue* queue)override;
	};
}