#pragma once
#include "../RHIClasses.h"

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

	class RHIVulkan;

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
	private:
		TVector<VkClearValue> m_Clears;
		friend RHIVulkan;
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
		friend RHIVulkan;
		VkImage handle;
	};

	class RImageViewVk: public RImageView {
	public:
		VkImageView handle;
		friend RHIVulkan;
	};

	class RFramebufferVk: public RFramebuffer {
	public:
		VkFramebuffer handle;
		friend RHIVulkan;
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

	class RPipelineVk: public RPipeline {
	public:
		VkPipeline handle;
		friend RHIVulkan;
	};

	class RDescriptorSetLayoutVk: public RDescriptorSetLayout {
	public:
		VkDescriptorSetLayout handle;
	};

	class RDescriptorSetVk: public RDescriptorSet {
	public:
		VkDescriptorSet handle;
	};

	class RPipelineLayoutVk: public RPipelineLayout {
	public:
		VkPipelineLayout handle;
	};

}