#pragma once

#ifdef USE_VMA
#include <vk_mem_alloc.h>
#else
#include <vulkan/vulkan.h>
#endif;
#include "../RHI.h"
#include "RHIVKResources.h"
#include "Core/Container/Container.h"

namespace RHI{
	class RHIVulkan final: public RHIInstance {
#pragma region rhi initialize
	private:
		uint8_t m_MaxFramesInFlight{ 3 };
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

		// physical devices's info
		uint32_t m_GraphicsIndex;
		uint32_t m_PresentIndex;
		uint32_t m_ComputeIndex;
		uint32_t m_ImageCount;
		VkSurfaceFormatKHR m_SwapchainFormat;
		VkPresentModeKHR m_SwapchainPresentMode;
		VkSurfaceTransformFlagBitsKHR m_SwapchainTransform;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties;

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
		VmaAllocator m_Vma{ nullptr };


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
		TVector<const char*> GetRequiredExtensions();
		void CreateInstance();
		void InitializeDebugMessenger();
		void CreateWindowSurface();
		void InitializePhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPools();
		void CreateCommandBuffers();
		void CreateDescriptorPool();
		void CreateSyncResources();
		void CreateMemoryAllocator();
		void CreateSwapchain();
		void ClearSwapchain();
#pragma endregion
		RImageVk* CreateImage(RImageType type, RFormat format, RSExtent3D&& extent, uint32_t mipLevels, uint32_t arrayLayers, 
			RSampleCountFlags samples, RImageTiling tiling, RImageUsageFlags usage);
	public:
		RHIVulkan() = default;
		void Initialize(const RSInitInfo* initInfo) override;
		void Release() override;
		RSVkImGuiInitInfo GetImGuiInitInfo();
		RCommandBuffer* GetCurrentCommandBuffer(uint8_t idx) override;
		uint8_t GetMaxFramesInFlight() override { return m_MaxFramesInFlight; }
		RFormat GetSwapchainImageFormat() override { return (RFormat)m_SwapchainFormat.format; }
		const RSExtent2D& GetSwapchainExtent() override { return m_SwapchainExtent; }
		RImageView* GetSwapchainImageView(uint8_t i) override;
		uint32_t GetSwapchainMaxImageCount() override;
		RQueue* GetGraphicsQueue() override;

		RRenderPass* CreateRenderPass(uint32_t attachmentCount, const RSAttachment* attachments) override;
		void DestroyRenderPass(RRenderPass* pass) override;
		RDescriptorSetLayout* CreateDescriptorSetLayout(uint32_t bindingCount, const RSDescriptorSetLayoutBinding* bindings)override;
		RDescriptorSet* AllocateDescriptorSet(const RDescriptorSetLayout* layout) override;
		void AllocateDescriptorSets(uint32_t count, const RDescriptorSetLayout* const* layouts, RDescriptorSet* const* descriptorSets)override;
		void FreeDescriptorSets(uint32_t count, RDescriptorSet** descriptorSets) override;

		void CreatePipelineLayout(uint32_t setLayoutCount, const RDescriptorSetLayout* const* pSetLayouts, uint32_t pushConstantRange, const RSPushConstantRange* pPushConstantRanges)override;

		void QueueSubmit(RQueue* queue,
			uint32_t cmdCount, RCommandBuffer* cmds,
			uint32_t waitSemaphoreCount, RSemaphore* waitSemaphores, RPipelineStageFlags* waitStageFlags,
			uint32_t signalSemaphoreCount, RSemaphore* signalSemaphores,
			RFence* fence) override;
		void QueueWaitIdle(RQueue* queue)override;
		RFramebuffer* CreateFrameBuffer(RRenderPass* pass, uint32_t imageViewCount, const RImageView* const* pImageViews, uint32_t width, uint32_t height, uint32_t layers) override;
		void DestoryFramebuffer(RFramebuffer* framebuffer) override;
		RCommandBuffer* AllocateCommandBuffer(RCommandBufferLevel level)override;
		void BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags) override;
		void EndCommandBuffer(RCommandBuffer* cmd) override;
		void FreeCommandBuffer(RCommandBuffer* cmd) override;
		void CmdBeginRenderPass(RCommandBuffer* cmd, RRenderPass* pass, RFramebuffer* framebuffer, RSRect2D renderArea, uint32_t clearValueCount, const RSClear* clearValues)override;
		void CmdEndRenderPass(RCommandBuffer* cmd) override;
		void CmdTransitionImageLayout(RCommandBuffer* cmd, RImage* image, RImageLayout oldLayout, RImageLayout newLayout,
			uint32_t baseLevel, uint32_t levelCount, uint32_t baseLayer, uint32_t layerCount, RImageAspectFlags aspect) override;
		void CmdCopyBufferToImage(RCommandBuffer* cmd, RBuffer* buffer, RImage* image, RImageAspectFlags aspect, uint32_t mipLevel, uint32_t baseLayout, uint32_t layerCount) override;
		void CmdBlitImage(RCommandBuffer* cmd, RImage* srcImage, RImage* dstImage, const RSImageBlit* pRegion) override;
		void CmdGenerateMipMap(RCommandBuffer* cmd, RImage* image, uint32_t levelCount, RImageAspectFlags aspect, uint32_t baseLayer, uint32_t layerCount) override;
		void ImmediateCommit(CommandBufferFunc func) override;
		uint32_t PrepareRendering(uint8_t frameIndex) override;
		void QueueSubmitRendering(RCommandBuffer* cmd, uint8_t frameIndex) override;

		void DestroyMemory(RMemory* memory)override;

		// buffer
		RBuffer* CreatBuffer(size_t size, RBufferUsage usage) override;
		void CreateBufferWithMemory(size_t size, RBufferUsage usage, RMemoryPropertyFlags memoryFlags,
			RBuffer*& pBuffer, RMemory*& pMemory, size_t dataSize, void* pData)override;
		void DestroyBuffer(RBuffer* buffer) override;

		// image
		RImage* CreateImage2D(RFormat format, uint32_t width, uint32_t height, uint32_t mipLevels,
		                      RSampleCountFlagBits samples, RImageTiling tiling, RImageUsageFlags usage) override;
		RMemory* CreateImageMemory(RImage* image, RMemoryPropertyFlags memoryProperty, void* pData) override;
		void DestroyImage(RImage* image) override;
		RImageView* CreateImageView(RImage* image, RImageViewType viewType, RImageAspectFlags aspectMask,
			uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseLayer, uint32_t layerCount) override;
		void DestroyImageView(RImageView* imageView) override;
		RSampler* CreateSampler(const RSSamplerInfo* samplerInfo) override;
	};
}