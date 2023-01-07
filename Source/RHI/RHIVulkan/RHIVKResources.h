#pragma once
#include "../RHIResources.h"

#ifdef USE_VMA
#include <vk_mem_alloc.h>
#else
#include <vulkan/vulkan.h>
#endif
namespace RHI {
#define RESOURCE_VK_HANDLE(cls, vkHandle)\
	class cls##Vk{\
	public:\
		vkHandle handle;\
	}

	struct RSVkImGuiInitInfo {
		void* windowHandle;
		VkInstance instance;
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		uint32_t queueIndex;
		VkQueue queue;
		VkDescriptorPool descriptorPool;
	};

	class RWindowHandleVk: public RWindowHandle {
	public:
		uint64_t handle;
	};

	class RCommandBufferVk: public RCommandBuffer {
	public:
		RCommandBufferVk() = default;
		VkCommandBuffer handle{VK_NULL_HANDLE};
		VkCommandPool m_Pool{ VK_NULL_HANDLE };
	};


	class RRenderPassVk: public RRenderPass {
	public:
		VkRenderPass handle;
	};

	class RMemoryVk: public RMemory {
	public:
		VkDeviceMemory handle;
		VkDeviceSize offset;
		VkDeviceSize size;
	};

	class RMemoryVma: public RMemory {
	public:
		VmaAllocation handle;
	};

	class RImageVk: public RImage {
	public:
		friend class RHIVulkan;
		VkImage handle;
	};

	class RImageViewVk: public RImageView {
	public:
		VkImageView handle;
	};

	class RFramebufferVk: public RFramebuffer {
	public:
		VkFramebuffer handle;
	};

	class RQueueVk: public RQueue {
	public:
		VkQueue handle;
	};

	class RSemaphoreVk: public RSemaphore {
	public:
		VkSemaphore handle;
	};

	class RFenceVk: public RFence {
	public:
		VkFence handle;
	};

	class RBufferVk: public RBuffer {
	public:
		VkBuffer handle;
	};

	class RSamplerVk: public RSampler {
	public:
		VkSampler handle;
	};
}