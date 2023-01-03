#pragma once
#include "../RHIResources.h"
#include <vulkan/vulkan.h>
namespace RHI {

	class RWindowHandleVk: public RWindowHandle {
	public:
		uint64_t m_WindowHandle;
	};

	class RCommandBufferVk: public RCommandBuffer {
	public:
		RCommandBufferVk() = default;
		VkCommandBuffer m_VkCmd{VK_NULL_HANDLE};
		VkCommandPool m_Pool{ VK_NULL_HANDLE };
	};

	class RRenderPassVk: public RRenderPass {
	public:
		VkRenderPass m_VkRenderPass;
	};

	class RImageViewVk: public RImageView {
	public:
		VkImageView m_VkImageView;
	};

	class RFramebufferVk: public RFramebuffer {
	public:
		VkFramebuffer m_VkFramebuffer;
	};

	class RQueueVk:public RQueue {
	public:
		VkQueue m_VkQueue;
	};

	class RSemaphoreVk: public RSemaphore {
	public:
		VkSemaphore m_VkSemaphore;
	};

	class RFenceVk: public RFence {
	public:
		VkFence m_VkFence;
	};

	class RBufferVk: public RBuffer {
	private:
		VkDevice m_Device;
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};

	struct RSVkImGuiInitInfo {
		void* windowHandle;
		VkInstance instance;
		VkDevice device;
		VkPhysicalDevice physicalDevice;
		uint32_t queueIndex;
		VkQueue queue;
		VkDescriptorPool descriptorPool;
	};
}