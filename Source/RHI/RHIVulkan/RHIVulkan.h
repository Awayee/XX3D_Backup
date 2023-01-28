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
		uint8 m_MaxFramesInFlight{ 3 };
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
		uint32 m_GraphicsIndex;
		uint32 m_PresentIndex;
		uint32 m_ComputeIndex;
		uint32 m_ImageCount;
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
		uint32 m_CurrentSwapchainImageIndex;
		TVector<VkImage> m_SwapchainImages;
		TVector<RImageViewVk> m_SwapchainImageViews;
		VkRect2D m_Scissor;

		bool m_IsRecording{ false }; // recording mode, some functions will not execute immediately

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
		PFN_vkCmdDraw				_vkCmdDraw;
		PFN_vkCmdDispatch			_vkCmdDispatch;
		PFN_vkCmdCopyBuffer			_vkCmdCopyBuffer;

	private:
		TVector<const char*> GetRequiredExtensions();
		void CreateInstance();
		void InitializeDebugMessenger();
		void CreateWindowSurface();
		void InitializePhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPools();
		void CreateDescriptorPool();
		void CreateSyncResources();
		void CreateMemoryAllocator();
		void CreateSwapchain();
		void ClearSwapchain();
#pragma endregion
		RImageVk* CreateImage(RImageType type, RFormat format, RSExtent3D&& extent, uint32 mipLevels, uint32 arrayLayers, 
			RSampleCountFlags samples, RImageTiling tiling, RImageUsageFlags usage);
	public:
		RHIVulkan() = default;
		void Initialize(const RSInitInfo* initInfo) override;
		void Release() override;
		RSVkImGuiInitInfo GetImGuiInitInfo();
		uint8 GetMaxFramesInFlight() override { return m_MaxFramesInFlight; }
		RFormat GetSwapchainImageFormat() override { return (RFormat)m_SwapchainFormat.format; }
		const RSExtent2D& GetSwapchainExtent() override { return m_SwapchainExtent; }
		RImageView* GetSwapchainImageView(uint8 i) override;
		uint32 GetSwapchainMaxImageCount() override;
		RQueue* GetGraphicsQueue() override;
		RFormat GetDepthFormat() override { return (RFormat)m_DepthFormat; };

		void ResizeSwapchain(uint32 width, uint32 height) override;

		void RecordBegin() override {};
		void RecordEnd() override {};

		RRenderPass* CreateRenderPass(uint32 attachmentCount, const RSAttachment* pAttachments,
			uint32 subpassCount, const RSubPassInfo* pSubpasses,
			uint32 dependencyCount, const RSubpassDependency* pDependencies) override;
		RRenderPass* CreateRenderPass(uint32 colorAttachmentCount, const RSAttachment* pColorAttachments, const RSAttachment* depthAttachment) override;
		void DestroyRenderPass(RRenderPass* pass) override;

		// descriptor set
		RDescriptorSetLayout* CreateDescriptorSetLayout(uint32 bindingCount, const RSDescriptorSetLayoutBinding* bindings)override;
		void DestroyDescriptorSetLayout(RDescriptorSetLayout* descriptorSetLayout) override;
		RDescriptorSet* AllocateDescriptorSet(const RDescriptorSetLayout* layout) override;
		//void AllocateDescriptorSets(uint32 count, const RDescriptorSetLayout* const* layouts, RDescriptorSet* const* descriptorSets)override;
		//void FreeDescriptorSets(uint32 count, RDescriptorSet** descriptorSets) override;
		void FreeDescriptorSet(RDescriptorSet* descriptorSet) override;
		void UpdateDescriptorSet(RDescriptorSet* descriptorSet, uint32 binding, uint32 arrayElement, uint32 count, RDescriptorType type, const RDescriptorInfo& descriptorInfo) override;

		// pipeline
		RPipelineLayout* CreatePipelineLayout(uint32 setLayoutCount, const RDescriptorSetLayout* const* pSetLayouts, uint32 pushConstantRangeCount, const RSPushConstantRange* pPushConstantRanges)override;
		void DestroyPipelineLayout(RPipelineLayout* pipelineLayout) override;
		RPipeline* CreateGraphicsPipeline(const RGraphicsPipelineCreateInfo& createInfo, RPipelineLayout* layout, RRenderPass* renderPass, uint32 subpass,
			RPipeline* basePipeline, int32_t basePipelineIndex) override;
		RPipeline* CreateComputePipeline(const RPipelineShaderInfo& shader, RPipelineLayout* layout, RPipeline* basePipeline, uint32 basePipelineIndex) override;
		void DestroyPipeline(RPipeline* pipeline) override;

		// command buffer
		void QueueSubmit(RQueue* queue,
			uint32 cmdCount, RCommandBuffer* cmds,
			uint32 waitSemaphoreCount, RSemaphore* waitSemaphores, RPipelineStageFlags* waitStageFlags,
			uint32 signalSemaphoreCount, RSemaphore* signalSemaphores,
			RFence* fence) override;
		void QueueWaitIdle(RQueue* queue)override;
		void WaitGraphicsQueue() override;
		RFramebuffer* CreateFrameBuffer(RRenderPass* pass, uint32 attachmentCount, const RImageView* const* pAttachments, uint32 width, uint32 height, uint32 layers) override;
		void DestroyFramebuffer(RFramebuffer* framebuffer) override;
		RCommandBuffer* AllocateCommandBuffer(RCommandBufferLevel level)override;
		void BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags) override;
		void EndCommandBuffer(RCommandBuffer* cmd) override;
		void FreeCommandBuffer(RCommandBuffer* cmd) override;
		void CmdBeginRenderPass(RCommandBuffer* cmd, RRenderPass* pass, RFramebuffer* framebuffer, RSRect2D area) override;
		void CmdNextSubpass(RCommandBuffer* cmd) override;
		void CmdEndRenderPass(RCommandBuffer* cmd) override;
		void CmdTransitionImageLayout(RCommandBuffer* cmd, RImage* image, RImageLayout oldLayout, RImageLayout newLayout,
			uint32 baseLevel, uint32 levelCount, uint32 baseLayer, uint32 layerCount, RImageAspectFlags aspect) override;
		void CmdCopyBufferToImage(RCommandBuffer* cmd, RBuffer* buffer, RImage* image, RImageAspectFlags aspect, uint32 mipLevel, uint32 baseLayout, uint32 layerCount) override;
		void CmdBlitImage(RCommandBuffer* cmd, RImage* srcImage, RImage* dstImage, const RSImageBlit* pRegion) override;
		void CmdGenerateMipMap(RCommandBuffer* cmd, RImage* image, uint32 levelCount, RImageAspectFlags aspect, uint32 baseLayer, uint32 layerCount) override;

		void CmdBindPipeline(RCommandBuffer* cmd, RPipeline* pipeline) override;
		void CmdBindDescriptorSet(RCommandBuffer* cmd, RPipelineType pipelineType, RPipelineLayout* layout, RDescriptorSet* descriptorSet, uint32 setIdx) override;
		void CmdBindVertexBuffer(RCommandBuffer* cmd, RBuffer* buffer, uint32 first, uint64 offset) override;
		void CmdBindIndexBuffer(RCommandBuffer* cmd, RBuffer* buffer, uint64 offset) override;
		void CmdDraw(RCommandBuffer* cmd, uint32 vertexCount, uint32 instanceCount, uint32 firstIndex, uint32 firstInstance) override;
		void CmdDrawIndexed(RCommandBuffer* cmd, uint32 indexCount, uint32 instanceCount, uint32 firstIndex, int32_t vertexOffset, uint32 firstInstance) override;
		void CmdDispatch(RCommandBuffer* cmd, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)override;
		void CmdClearAttachment(RCommandBuffer* cmd, RImageAspectFlags aspect, const float* color, const RSRect2D& rect) override;
		void CmdCopyBuffer(RCommandBuffer* cmd, RBuffer* srcBuffer, RBuffer* dstBuffer, uint64 srcOffset, uint64 dstOffset, uint64 size) override;
		void CmdBeginDebugLabel(RCommandBuffer* cmd, const char* msg, const float* color) override;
		void CmdEndDebugLabel(RCommandBuffer* cmd) override;

		void ImmediateCommit(const CommandBufferFunc& func) override;
		int PreparePresent(uint8 frameIndex) override;
		int QueueSubmitPresent(RCommandBuffer* cmd, uint8 frameIndex) override;

		// buffer
		RBuffer* CreateBuffer(uint64 size, RBufferUsageFlags usage) override;
		RMemory* CreateBufferMemory(RBuffer* buffer, RMemoryPropertyFlags memoryProperty, uint64 dataSize, void* pData) override;
		void CreateBufferWithMemory(uint64 size, RBufferUsageFlags usage, RMemoryPropertyFlags memoryFlags,
			RBuffer*& pBuffer, RMemory*& pMemory, uint64 dataSize, void* pData)override;
		void DestroyBuffer(RBuffer* buffer) override;
		void MapMemory(RMemory* memory, void** pData) override;
		void UnmapMemory(RMemory* memory) override;

		// image
		RImage* CreateImage2D(RFormat format, uint32 width, uint32 height, uint32 mipLevels,
		                      RSampleCountFlagBits samples, RImageTiling tiling, RImageUsageFlags usage) override;
		RMemory* CreateImageMemory(RImage* image, RMemoryPropertyFlags memoryProperty, void* pData) override;
		void DestroyImage(RImage* image) override;
		RImageView* CreateImageView(RImage* image, RImageViewType viewType, RImageAspectFlags aspectMask,
			uint32 baseMipLevel, uint32 levelCount, uint32 baseLayer, uint32 layerCount) override;
		void DestroyImageView(RImageView* imageView) override;
		RSampler* CreateSampler(const RSSamplerInfo& samplerInfo) override;

		void FreeMemory(RMemory* memory)override;
	};
}