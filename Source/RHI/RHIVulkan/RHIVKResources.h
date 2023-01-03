#pragma once
#include "../RHIResources.h"
#include <vulkan/vulkan.h>
namespace RHI {
	class RVkWindowHandle: public RWindowHandle {
	public:
		uint64_t m_WindowHandle;
	};

	class RVkCommandBuffer: public RCommandBuffer {
	public:
		RVkCommandBuffer() = default;
		VkCommandBuffer m_VkCmd{VK_NULL_HANDLE};
	};

	class RVkRenderPass: public RRenderPass {
	public:
		VkRenderPass m_VkRenderPass;
	};

	class RVkQueue:public RQueue {
	public:
		VkQueue m_VkQueue;
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

	class RVkSemaphore: public RSemaphore {
	public:
		VkSemaphore m_VkSemaphore;
	};

	class RVkFence: public RFence {
	public:
		VkFence m_VkFence;
	};

	class RVkBuffer: public RBuffer {
	private:
		VkDevice m_Device;
		VkBuffer m_Buffer;
		VkDeviceMemory m_BufferMemory;
	};
}