#pragma once
#include "RHIResources.h"
#include "Core/Container/Container.h"

namespace RHI{
	typedef void(*DebugFunc)(const char*);

	class RHIInstance
	{
	public:
		virtual void Initialize(const RSInitInfo* initInfo)=0;
		virtual void Release() = 0;
		virtual RCommandBuffer* GetCurrentCommandBuffer(uint8_t idx) = 0;
		virtual uint8_t GetMaxFramesInFlight() = 0;
		virtual const RSExtent2D& GetSwapchainExtent() = 0;
		virtual RFormat GetSwapchainImageFormat() = 0;
		virtual RImageView* GetSwapchainImageView(uint8_t i) = 0;
		virtual uint32_t GetSwapchainMaxImageCount() = 0;

		virtual RRenderPass* CreateRenderPass(uint32_t attachmentCount, const RSAttachment* attachments) = 0;
		virtual void DestroyRenderPass(RRenderPass* pass) = 0;
		virtual RDescriptorSetLayout* CreateDescriptorSetLayout(uint32_t bindingCount, const RSDescriptorSetLayoutBinding* bindings) = 0;
		virtual RDescriptorSet* AllocateDescriptorSet(const RDescriptorSetLayout* layout) = 0;
		virtual void FreeDescriptorSets(uint32_t count, RDescriptorSet** descriptorSets) = 0;
		virtual void AllocateDescriptorSets(uint32_t count, const RDescriptorSetLayout* const* layouts, RDescriptorSet*const* descriptorSets) = 0;
		virtual void CreatePipelineLayout(uint32_t setLayoutCount, const RDescriptorSetLayout*const* pSetLayouts, uint32_t pushConstantRange, const RSPushConstantRange* pPushConstantRanges) = 0;
		//virtual RPipeline* CreateGraphicsPipeline() = 0;

		virtual RQueue* GetGraphicsQueue() = 0;
		virtual void QueueSubmit(RQueue* queue, 
			uint32_t cmdCount, RCommandBuffer* cmds, 
			uint32_t waitSemaphoreCount, RSemaphore* waitSemaphores, RPipelineStageFlags* waitStageFlags, 
			uint32_t signalSemaphoreCount, RSemaphore* signalSemaphores, 
			RFence* fence) = 0;
		virtual void QueueWaitIdle(RQueue* queue) = 0;
		virtual RFramebuffer* CreateFrameBuffer(RRenderPass* pass, uint32_t imageViewCount, const RImageView* const* pImageViews, uint32_t width, uint32_t height, uint32_t layers) = 0;
		virtual void DestoryFramebuffer(RFramebuffer* framebuffer) = 0;
		// cmd
		virtual RCommandBuffer* AllocateCommandBuffer(RCommandBufferLevel level) = 0;
		virtual void BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags) = 0;
		virtual void EndCommandBuffer(RCommandBuffer* cmd) = 0;
		virtual void FreeCommandBuffer(RCommandBuffer* cmd) = 0;
		virtual void CmdBeginRenderPass(RCommandBuffer* cmd, RRenderPass* pass, RFramebuffer* framebuffer,
			RSRect2D renderArea, uint32_t clearValueCount, const RSClear* clearValues) = 0;

		virtual void CmdEndRenderPass(RCommandBuffer* cmd) = 0;

		virtual void CmdTransitionImageLayout(RCommandBuffer* cmd, RImage* image, RImageLayout oldLayout, RImageLayout newLayout,
			uint32_t baseMipLevel, uint32_t levelCount, uint32_t layer, uint32_t layerCount, RImageAspectFlags aspect) = 0;
		virtual void CmdCopyBufferToImage(RCommandBuffer* cmd, RBuffer* buffer, RImage* image, RImageAspectFlags aspect, uint32_t mipLevel, uint32_t baseLayout, uint32_t layerCount) = 0;
		virtual void CmdBlitImage(RCommandBuffer* cmd, RImage* srcImage, RImage* dstImage, const RSImageBlit* pRregion) = 0;
		virtual void CmdGenerateMipMap(RCommandBuffer* cmd, RImage* image, uint32_t levelCount, RImageAspectFlags aspect, uint32_t baseLayer, uint32_t layerCount) = 0;

		typedef void(*CommandBufferFunc)(RCommandBuffer*);
		virtual void ImmediateCommit(CommandBufferFunc func) = 0;

		virtual uint32_t PrepareRendering(uint8_t frameIndex) = 0; // return image index of the swapchain
		virtual void QueueSubmitRendering(RCommandBuffer* cmd, uint8_t frameIndex) = 0;

		virtual void DestroyMemory(RMemory* memory) = 0;

		// buffer
		virtual RBuffer* CreatBuffer(size_t size, RBufferUsage usage) = 0;
		virtual void CreateBufferWithMemory(size_t size, RBufferUsage usage, RMemoryPropertyFlags memoryFlags,
			RBuffer*& pBuffer, RMemory*& pMemory, size_t dataSize, void* data) = 0;
		virtual void DestroyBuffer(RBuffer* buffer) = 0;

		// image
		virtual RImage* CreateImage2D(RFormat format, uint32_t width, uint32_t height, uint32_t mipLevels,
			RSampleCountFlagBits samples, RImageTiling tiling, RImageUsageFlags usage) = 0;
		virtual RMemory* CreateImageMemory(RImage* image, RMemoryPropertyFlags memoryProperty, void* data) = 0;
		virtual void DestroyImage(RImage* image) = 0;
		virtual RImageView* CreateImageView(RImage* image, RImageViewType viewType, RImageAspectFlags aspectMast,
			uint32_t baseMiplevel, uint32_t levelCount, uint32_t baseLayer,uint32_t layerCount) = 0;
		virtual void DestroyImageView(RImageView* imageView) = 0;
		virtual RSampler* CreateSampler(const RSSamplerInfo* samplerInfo) = 0;

	};
}
