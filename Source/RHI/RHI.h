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
		virtual void CreateRenderPass(RRenderPass* pass, uint32_t attachmentCount, RSAttachment* attachments) = 0;
		virtual void DestroyRenderPass(RRenderPass* pass) = 0;
		virtual RCommandBuffer* CreateCommandBuffer(RCommandBufferLevel level) = 0;
		virtual void BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags) = 0;
		virtual void EndCommandBuffer(RCommandBuffer* cmd) = 0;
		virtual void FreeCommandBuffer(RCommandBuffer*& cmd) = 0;
		virtual RQueue* GetGraphicsQueue() = 0;
		virtual void QueueSubmit(RQueue* queue, TVector<RCommandBuffer*> cmds, TVector<TPair<RSemaphore*, RPipelineStageFlags>> waitSemaphores, TVector<RSemaphore*> signalSemaphores, RFence* fence) = 0;
		virtual void QueueWaitIdle(RQueue* queue) = 0;
	};
}
