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
		virtual RCommandBuffer* GetCurrentCommandBuffer() = 0;
		virtual uint8_t GetMaxFramesInFlight() = 0;
		virtual const RSExtent2D& GetSwapchainExtent() = 0;
		virtual RFormat GetSwapchainImageFormat() = 0;
		virtual RRenderPass* CreateRenderPass(uint32_t attachmentCount, RSAttachment* attachments) = 0;
		virtual void DestroyRenderPass(RRenderPass* pass) = 0;
		virtual RQueue* GetGraphicsQueue() = 0;
		virtual void QueueSubmit(RQueue* queue, 
			uint32_t cmdCount, RCommandBuffer* cmds, 
			uint32_t waitSemaphoreCount, RSemaphore* waitSemaphores, RPipelineStageFlags* waitStageFlags, 
			uint32_t signalSemaphoreCount, RSemaphore* signalSemaphores, 
			RFence* fence) = 0;
		virtual void QueueWaitIdle(RQueue* queue) = 0;
		virtual RImageView* GetSwapchainImageViews(uint8_t i) = 0;
		virtual RFramebuffer* CreateFrameBuffer(RRenderPass* pass, const TVector<RImageView*>& imageViews, uint32_t width, uint32_t height, uint32_t layers) = 0;
		virtual void DestoryFramebuffer(RFramebuffer* framebuffer) = 0;
		// cmd
		virtual RCommandBuffer* CreateCommandBuffer(RCommandBufferLevel level) = 0;
		virtual void BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags) = 0;
		virtual void EndCommandBuffer(RCommandBuffer* cmd) = 0;
		virtual void FreeCommandBuffer(RCommandBuffer* cmd) = 0;
		virtual void CmdBeginRenderPass(RCommandBuffer* cmd, RRenderPass* pass, RFramebuffer* framebuffer, RSRect2D renderArea, uint32_t clearValueCount, const RSClearValue* clearValues) = 0;
		virtual void CmdEndRenderPass(RCommandBuffer* cmd) = 0;

		typedef void(*CmdFunc)(RCommandBuffer* cmd);

		virtual void ImmediateSubmit(CmdFunc func) = 0;

		virtual void PrepareRendering() = 0;
		virtual void QueueSubmitRendering(RCommandBuffer* cmd) = 0;
	};
}
