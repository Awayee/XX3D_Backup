#include "RHIVulkan.h"
#include "VulkanUtil.hpp"
#include <GLFW/glfw3.h>
#include "Core/macro.h"
#ifdef USE_VMA
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#else
#include <vulkan/vulkan.h>
#endif

namespace RHI {

#define VK_CHECK(x, s)\
	if(VK_SUCCESS != x) throw s

	const uint32_t VK_API_VER = VK_API_VERSION_1_2;

#define VK_FREE(ptr) delete ptr; ptr = nullptr

#pragma region private members
	// debug callback
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* data)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	void SetDebugInfo(VkDebugUtilsMessengerCreateInfoEXT& debugInfo) {
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugInfo.pfnUserCallback = DebugCallback;
	}

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT     debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	void FindQueueFamilyIndex(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, int* pGraphicsIndex, int* pPresentIndex, int* pComputeIndex)
	{
		*pGraphicsIndex = -1;
		*pPresentIndex = -1;
		*pComputeIndex = -1;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		TVector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
		for (uint32_t i = 0; i < queueFamilyCount; i++) {
			if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				*pGraphicsIndex = i;
			}
			if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				*pComputeIndex = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				*pPresentIndex = i;
			}

			if (*pGraphicsIndex != -1 && *pPresentIndex != -1 && *pComputeIndex != -1) break;
		}
	}

	bool CheckExtensionSupported(VkPhysicalDevice device, const TVector<const char*>& extensions) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		TVector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
		bool isSupport = false;
		for (const char* extensionName : extensions) {
			bool flag = false;
			for (auto& extension : availableExtensions) {
				if (strcmp(extensionName, extension.extensionName) == 0) {
					flag = true;
					isSupport = true;
					break;
				}
			}
			if (flag) break;
		}
		return isSupport;
	}

	bool RHIVulkan::CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		TVector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		for (const char* layerName : m_ValidationLayers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}
	}
	TVector<const char*> RHIVulkan::GetRequiredExtensions()
	{
		uint32_t     glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		TVector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (m_EnableValidationLayers || m_EnableDebugUtils)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
	}
	void RHIVulkan::CreateInstance()
	{
		ASSERT(!m_EnableValidationLayers || CheckValidationLayerSupport(), "validation layers requested, but not available!");
		// app info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = m_AppName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "xxEngine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VER;

		// create info
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo; // the appInfo is stored here

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if(m_EnableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
			createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
			SetDebugInfo(debugCreateInfo);
			debugCreateInfo.pUserData = (void*)this;
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// create m_vulkan_context._instance
		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_Instance), "");
	}
	void RHIVulkan::InitializeDebugMessenger()
	{
		if(m_EnableValidationLayers) {
			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			SetDebugInfo(createInfo);
			createInfo.pUserData = (void*)this;
			VK_CHECK(CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger), "vk create debug messenger!");
		}

		if(m_EnableDebugUtils) {
			_vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdBeginDebugUtilsLabelEXT");
			_vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_Instance, "vkCmdEndDebugUtilsLabelEXT");
		}
	}
	void RHIVulkan::CreateWindowSurface()
	{
		VK_CHECK(glfwCreateWindowSurface(m_Instance, reinterpret_cast<GLFWwindow*>(m_WindowHandle), nullptr, &m_Surface), "vk create window surface");
	}

	RHIVulkan::SPhyicalDeviceInfo RHIVulkan::GetPhysicalDeviceInfo(VkPhysicalDevice physicalDevice)
	{
		RHIVulkan::SPhyicalDeviceInfo info;
		if (!CheckExtensionSupported(physicalDevice, m_DeviceExtensions)) {
			return info;
		}
		// query family index
		FindQueueFamilyIndex(physicalDevice, m_Surface, &info.graphicsIndex, &info.presentIndex, &info.computeIndex);
		if(-1 == info.graphicsIndex || -1 == info.presentIndex || -1 == info.computeIndex) {
			return info;
		}

		// query swapchain info
		// format
		uint32_t formatCount;
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, nullptr), "vkGetPhysicalDeviceSurfaceFormatsKHR");
		TVector<VkSurfaceFormatKHR> formats(formatCount);
		VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, formats.data()), "vkGetPhysicalDeviceSurfaceFormatsKHR");
		for (const auto& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				info.swapchainFormat = format;
				break;
			}
		}
		//present mode
		uint32_t presentModeCount;
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, nullptr), "vkGetPhysicalDeviceSurfacePresentModesKHR");
		TVector<VkPresentModeKHR> presentModes(presentModeCount);
		VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data()), "vkGetPhysicalDeviceSurfacePresentModesKHR");
		for (const auto& mode : presentModes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				info.swapchainPresentMode = mode;
				break;
			}
		}

		//extent
		VkSurfaceCapabilitiesKHR capabilities;
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &capabilities), "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			info.swapchainExtent = capabilities.currentExtent;	
		}
		info.imageCount = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && info.imageCount > capabilities.maxImageCount) {
			info.imageCount = capabilities.maxImageCount;
		}
		info.swapchainTransform = capabilities.currentTransform;

		if(info.swapchainFormat.format == VK_FORMAT_UNDEFINED || info.swapchainPresentMode == VK_PRESENT_MODE_MAX_ENUM_KHR || info.swapchainExtent.width == 0) {
			return info;
		}

		info.score = 1;

		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			info.score += 100;
		}
		else if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
			info.score += 10;
		}
		VkPhysicalDeviceFeatures features;
		vkGetPhysicalDeviceFeatures(physicalDevice, &features);
		if (features.geometryShader) {
			info.score += 10;
		}

		return info;
	}

	void RHIVulkan::InitializePhysicalDevice()
	{
		uint32_t deviceCount = 0;
		VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr), "no avaliable physical device");
		TVector<VkPhysicalDevice> devices(deviceCount);
		VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data()), "no avaliable physical device");
		TVector<TPair<VkPhysicalDevice, RHIVulkan::SPhyicalDeviceInfo>> devicesInfo;
		for (auto& device : devices) {
			auto deviceInfo = GetPhysicalDeviceInfo(device);
			if(deviceInfo.score > 0) {
				devicesInfo.push_back({device, std::move(deviceInfo)});
			}
		}

		ASSERT(!devicesInfo.empty(), "no avaliable GPU!");

		TSort(devicesInfo.begin(), devicesInfo.end(),
			[](const TPair<VkPhysicalDevice, RHIVulkan::SPhyicalDeviceInfo> p1, const TPair<VkPhysicalDevice, RHIVulkan::SPhyicalDeviceInfo> p2) {return p1.second.score > p2.second.score; });

		m_PhysicalDevice = devicesInfo[0].first;
		m_PhysicalDeviceInfo = devicesInfo[0].second;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
		LOG("Picked GPU: %s %u", m_PhysicalDeviceProperties.deviceName, m_PhysicalDeviceProperties.deviceID);
	}

	VkFormat RHIVulkan::FindDepthFormat()
	{

		// find depth format
		const TVector<VkFormat> candidates{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
		VkImageTiling tiling{ VK_IMAGE_TILING_OPTIMAL };
		VkFormatFeatureFlags features{ VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT };
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		ERROR("findSupportedFormat failed");
		return VK_FORMAT_UNDEFINED;
	}

	void RHIVulkan::CreateLogicalDevice()
	{
		TVector<VkDeviceQueueCreateInfo> queueCreateInfos;
		TUnorderedSet<int> queueFamilies{ m_PhysicalDeviceInfo.graphicsIndex, m_PhysicalDeviceInfo.computeIndex, m_PhysicalDeviceInfo.presentIndex };

		float queuePriority = 1.0f;
		for(int queueFamily: queueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = static_cast<uint32_t>(queueFamily);
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(std::move(queueCreateInfo));
		}

		VkPhysicalDeviceFeatures features{};
		features.samplerAnisotropy = VK_TRUE;
		features.fragmentStoresAndAtomics = VK_TRUE;
		features.independentBlend = VK_TRUE;
		features.geometryShader = m_EnableGeometryShader;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &features;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
		deviceCreateInfo.enabledLayerCount = 0;

		VK_CHECK(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device), "vkCreateDevice");

		// initialize queues of this device
		vkGetDeviceQueue(m_Device, static_cast<uint32_t>(m_PhysicalDeviceInfo.graphicsIndex), 0, &m_GraphicsQueue.handle);
		vkGetDeviceQueue(m_Device, static_cast<uint32_t>(m_PhysicalDeviceInfo.computeIndex), 0, &m_ComputeQueue.handle);
		vkGetDeviceQueue(m_Device, static_cast<uint32_t>(m_PhysicalDeviceInfo.presentIndex), 0, &m_PresentQueue.handle);

		// more efficient pointer
		_vkResetCommandPool = (PFN_vkResetCommandPool)vkGetDeviceProcAddr(m_Device, "vkResetCommandPool");
		_vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)vkGetDeviceProcAddr(m_Device, "vkBeginCommandBuffer");
		_vkEndCommandBuffer = (PFN_vkEndCommandBuffer)vkGetDeviceProcAddr(m_Device, "vkEndCommandBuffer");
		_vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)vkGetDeviceProcAddr(m_Device, "vkCmdBeginRenderPass");
		_vkCmdNextSubpass = (PFN_vkCmdNextSubpass)vkGetDeviceProcAddr(m_Device, "vkCmdNextSubpass");
		_vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)vkGetDeviceProcAddr(m_Device, "vkCmdEndRenderPass");
		_vkCmdBindPipeline = (PFN_vkCmdBindPipeline)vkGetDeviceProcAddr(m_Device, "vkCmdBindPipeline");
		_vkCmdSetViewport = (PFN_vkCmdSetViewport)vkGetDeviceProcAddr(m_Device, "vkCmdSetViewport");
		_vkCmdSetScissor = (PFN_vkCmdSetScissor)vkGetDeviceProcAddr(m_Device, "vkCmdSetScissor");
		_vkWaitForFences = (PFN_vkWaitForFences)vkGetDeviceProcAddr(m_Device, "vkWaitForFences");
		_vkResetFences = (PFN_vkResetFences)vkGetDeviceProcAddr(m_Device, "vkResetFences");
		_vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)vkGetDeviceProcAddr(m_Device, "vkCmdDrawIndexed");
		_vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)vkGetDeviceProcAddr(m_Device, "vkCmdBindVertexBuffers");
		_vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)vkGetDeviceProcAddr(m_Device, "vkCmdBindIndexBuffer");
		_vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)vkGetDeviceProcAddr(m_Device, "vkCmdBindDescriptorSets");
		_vkCmdClearAttachments = (PFN_vkCmdClearAttachments)vkGetDeviceProcAddr(m_Device, "vkCmdClearAttachments");

		m_DepthFormat = FindDepthFormat();
	}

	void RHIVulkan::CreateCommandPools()
	{
		// graphics command pool
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo{};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = NULL;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = static_cast<uint32_t>(m_PhysicalDeviceInfo.graphicsIndex);
			VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_RHICommandPool), "VkCreateCommandPool");
		}

		// other command pool
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = NULL;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = static_cast<uint32_t>(m_PhysicalDeviceInfo.graphicsIndex);
			m_CommandPools.resize(m_MaxFramesInFlight);
			for (uint32_t i = 0; i < m_MaxFramesInFlight; ++i) {
				VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPools[i]), "VkCreateCommandPool");
			}
		}
	}

	void RHIVulkan::CreateCommandBuffers()
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1U;
		m_CommandBuffers.resize(m_MaxFramesInFlight);
		for (uint32_t i = 0; i < m_MaxFramesInFlight; ++i)
		{
			commandBufferAllocateInfo.commandPool = m_CommandPools[i];
			VK_CHECK(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_CommandBuffers[i].handle), "vkAllocateCommandBuffers");
			m_CommandBuffers[i].m_Pool = commandBufferAllocateInfo.commandPool;
		}
	}

	void RHIVulkan::CreateDescriptorPool()
	{
		// Since DescriptorSet should be treated as asset in Vulkan, DescriptorPool
		// should be big enough, and thus we can sub-allocate DescriptorSet from
		// DescriptorPool merely as we sub-allocate Buffer/Image from DeviceMemory.

		uint32_t maxVertexBlendingMeshCount{ 256 };
		uint32_t maxMaterialCount{ 256 };

		TVector<VkDescriptorPoolSize> poolSizes(7);
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		poolSizes[0].descriptorCount = 3 + 2 + 2 + 2 + 1 + 1 + 3 + 3;
		poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[1].descriptorCount = 1 + 1 + 1 * maxVertexBlendingMeshCount;
		poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[2].descriptorCount = 1 * maxMaterialCount;
		poolSizes[3].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[3].descriptorCount = 3 + 5 * maxMaterialCount + 1 + 1; // ImGui_ImplVulkan_CreateDeviceObjects
		poolSizes[4].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		poolSizes[4].descriptorCount = 4 + 1 + 1 + 2;
		poolSizes[5].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		poolSizes[5].descriptorCount = 3;
		poolSizes[6].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSizes[6].descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1 + 1 + 1 + maxMaterialCount + maxVertexBlendingMeshCount + 1 + 1; // +skybox + axis descriptor set
		poolInfo.flags = 0U;
		VK_CHECK(vkCreateDescriptorPool(m_Device, &poolInfo, nullptr, &m_DescriptorPool), "VkCreateDescriptorPool");
	}

	void RHIVulkan::CreateSyncResources()
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // the fence is initialized as signaled

		m_ImageAvaliableSemaphores.resize(m_MaxFramesInFlight);
		m_PresentationFinishSemaphores.resize(m_MaxFramesInFlight);
		m_ImageAvaliableForTextureCopySemaphores.resize(m_MaxFramesInFlight);
		m_IsFrameInFlightFences.resize(m_MaxFramesInFlight);
		for(uint32_t i=0; i< m_MaxFramesInFlight; ++i) {
			VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_ImageAvaliableSemaphores[i]), "ImageAvaliableSemaphore");
			VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_PresentationFinishSemaphores[i]), "PresentationFinishSemaphore");
			VK_CHECK(vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &m_ImageAvaliableForTextureCopySemaphores[i]), "ImageAvaliableForTextureCopySemaphore");
			VK_CHECK(vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &m_IsFrameInFlightFences[i]), "vkCreateFence");
		}
	}
	void RHIVulkan::CreateMemoryAllocator()
	{
		VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocatorCreateInfo = {};
		allocatorCreateInfo.vulkanApiVersion = VK_API_VER;
		allocatorCreateInfo.physicalDevice = m_PhysicalDevice;
		allocatorCreateInfo.device = m_Device;
		allocatorCreateInfo.instance = m_Instance;
		allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;
		vmaCreateAllocator(&allocatorCreateInfo, &m_Vma);
	}
	void RHIVulkan::CreateSwapchain()
	{
		VkSwapchainCreateInfoKHR createInfo{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		createInfo.surface = m_Surface;
		createInfo.minImageCount = m_PhysicalDeviceInfo.imageCount;
		createInfo.imageFormat = m_PhysicalDeviceInfo.swapchainFormat.format;
		createInfo.imageColorSpace = m_PhysicalDeviceInfo.swapchainFormat.colorSpace;
		createInfo.imageExtent = m_PhysicalDeviceInfo.swapchainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t queueFamilyIndices[] = { static_cast<uint32_t>(m_PhysicalDeviceInfo.graphicsIndex), static_cast<uint32_t>(m_PhysicalDeviceInfo.presentIndex)};

		if (queueFamilyIndices[0] != queueFamilyIndices[1])
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		createInfo.preTransform = m_PhysicalDeviceInfo.swapchainTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PhysicalDeviceInfo.swapchainPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		VK_CHECK(vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain), "CreateSwapchain");
		// swapchain images
		uint32_t image_count;
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &image_count, nullptr);
		m_SwapchainImages.resize(image_count);
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &image_count, m_SwapchainImages.data());
		m_SwapchainExtent.width = m_PhysicalDeviceInfo.swapchainExtent.width;
		m_SwapchainExtent.height = m_PhysicalDeviceInfo.swapchainExtent.height;
		m_Scissor = { {0, 0,}, {m_SwapchainExtent.width, m_SwapchainExtent.height} };
		// Create image views
		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		// create imageview (one for each this time) for all swapchain images
		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imageViewCreateInfo.image = m_SwapchainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_PhysicalDeviceInfo.swapchainFormat.format;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			VK_CHECK(vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_SwapchainImageViews[i].handle), "CreateImageView");
		}
	}
	void RHIVulkan::ClearSwapchain()
	{
		for(auto imageView: m_SwapchainImageViews) {
			vkDestroyImageView(m_Device, imageView.handle, nullptr);
		}
		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
		m_Swapchain = nullptr;
	}
#pragma endregion

	void RHIVulkan::Initialize(const RSInitInfo* initInfo)
	{
		if(m_Initialized) {
			return;
		}

		m_EnableValidationLayers = initInfo->enableDebug;
		m_EnableDebugUtils = initInfo->enableDebug;
		m_EnableGeometryShader = initInfo->enableGeometryShader;
		m_AppName = initInfo->applicationName;
		m_WindowHandle = initInfo->windowHandle;
		m_MaxFramesInFlight = initInfo->maxFramesInFlight;

		CreateInstance();
		InitializeDebugMessenger();
		CreateWindowSurface();
		InitializePhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPools();
		CreateCommandBuffers();
		CreateDescriptorPool();
		CreateSyncResources();
		CreateSwapchain();
		CreateMemoryAllocator();

		m_Initialized = true;

		LOG("RHI: Vulkan is initialized.");
	}

	void RHIVulkan::Release()
	{
		//vma
		vmaDestroyAllocator(m_Vma);
		m_Vma = nullptr;
		//sync
		for (auto smp : m_ImageAvaliableSemaphores) {
			vkDestroySemaphore(m_Device, smp, nullptr);
		}
		m_ImageAvaliableSemaphores.clear();
		for (auto smp : m_PresentationFinishSemaphores) {
			vkDestroySemaphore(m_Device, smp, nullptr);
		}
		m_PresentationFinishSemaphores.clear();
		for (auto smp : m_ImageAvaliableForTextureCopySemaphores) {
			vkDestroySemaphore(m_Device, smp, nullptr);
		}
		m_ImageAvaliableForTextureCopySemaphores.clear();
		for (auto fence : m_IsFrameInFlightFences) {
			vkDestroyFence(m_Device, fence, nullptr);
		}
		m_IsFrameInFlightFences.clear();
		for (auto pool : m_CommandPools) {
			vkDestroyCommandPool(m_Device, pool, nullptr);
		}
		m_CommandPools.clear();
		vkDestroyCommandPool(m_Device, m_RHICommandPool, nullptr);
		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
		for (auto imageView : m_SwapchainImageViews) {
			vkDestroyImageView(m_Device, imageView.handle, nullptr);
		}
		m_SwapchainImageViews.clear();
		vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
		m_Swapchain = nullptr;
		vkDestroyDevice(m_Device, nullptr);
		m_Device = nullptr;
		m_Initialized = false;
	}

	RSVkImGuiInitInfo RHIVulkan::GetImGuiInitInfo()
	{
		RSVkImGuiInitInfo info;
		info.windowHandle = m_WindowHandle;
		info.instance = m_Instance;
		info.device = m_Device;
		info.physicalDevice = m_PhysicalDevice;
		info.queueIndex = static_cast<uint32_t>(m_PhysicalDeviceInfo.graphicsIndex);
		info.queue = m_GraphicsQueue.handle;
		info.descriptorPool = m_DescriptorPool;
		return info;
	}

	RCommandBuffer* RHIVulkan::GetCurrentCommandBuffer()
	{
		return static_cast<RCommandBuffer*>(&m_CommandBuffers[m_CurrentFrame]);
	}
	RRenderPass* RHIVulkan::CreateRenderPass(uint32_t attachmentCount, const RSAttachment* attachments)
	{
		VkRenderPassCreateInfo info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };
		info.flags = 0;
		info.attachmentCount = attachmentCount;
		TVector<VkAttachmentDescription> descs(attachmentCount);
		TVector<VkAttachmentReference> colorRefs;
		std::unique_ptr<VkAttachmentReference> depthRefPtr;

		for(uint32_t i=0; i<attachmentCount; ++i) {
			descs[i].flags = 0;
			descs[i].format = (VkFormat)attachments[i].format;
			descs[i].samples = VK_SAMPLE_COUNT_1_BIT;
			descs[i].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			descs[i].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			descs[i].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			descs[i].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			descs[i].initialLayout = (VkImageLayout)attachments[i].initialLayout;
			descs[i].finalLayout = (VkImageLayout)attachments[i].finalLayout;

			if(attachments[i].type == ATTACHMENT_COLOR) {
				colorRefs.push_back({i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
			}
			else if (attachments[i].type == ATTACHMENT_DEPTH) {
				depthRefPtr.reset(new VkAttachmentReference{i, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL });
			}
		}
		info.pAttachments = descs.data();

		// just 1 subpass
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = static_cast<uint32_t>(colorRefs.size());
		subpass.pColorAttachments = colorRefs.data();
		subpass.pDepthStencilAttachment = depthRefPtr.get();
		info.subpassCount = 1;
		info.pSubpasses = &subpass;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;

		VkRenderPass handle;
		if(VK_SUCCESS != vkCreateRenderPass(m_Device, &info, nullptr, &handle)) {
			return nullptr;
		}
		RRenderPassVk* pass = new RRenderPassVk;
		pass->handle = handle;
		return reinterpret_cast<RRenderPass*>(pass);
	}
	void RHIVulkan::DestroyRenderPass(RRenderPass* pass)
	{
		RRenderPassVk* vkPass = static_cast<RRenderPassVk*>(pass);
		vkDestroyRenderPass(m_Device, vkPass->handle, nullptr);
		delete vkPass;
	}
	RQueue* RHIVulkan::GetGraphicsQueue()
	{
		return reinterpret_cast<RQueue*>(&m_GraphicsQueue);
	}
	void RHIVulkan::QueueSubmit(RQueue* queue,
		uint32_t cmdCount, RCommandBuffer* cmds,
		uint32_t waitSemaphoreCount, RSemaphore* waitSemaphores, RPipelineStageFlags* waitStageFlags,
		uint32_t signalSemaphoreCount, RSemaphore* signalSemaphores,
		RFence* fence) {
		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.pNext = nullptr;
		uint32_t i;

		// wait semaphores
		TVector<VkSemaphore> vkWaitSmps(waitSemaphoreCount);
		for (i = 0; i < waitSemaphoreCount; ++i) {
			vkWaitSmps[i] = reinterpret_cast<RSemaphoreVk*>(waitSemaphores + i)->handle;
		}
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vkWaitSmps.size());
		submitInfo.pWaitSemaphores = vkWaitSmps.data();
		submitInfo.pWaitDstStageMask = (VkPipelineStageFlags*)waitStageFlags;

		// commands
		TVector<VkCommandBuffer> vkCmds(cmdCount);
		for (i = 0; i < cmdCount; ++i) {
			vkCmds[i] = reinterpret_cast<RCommandBufferVk*>(cmds + i)->handle;
		}
		submitInfo.commandBufferCount = cmdCount;
		submitInfo.pCommandBuffers = vkCmds.data();

		// signal semaphores
		TVector<VkSemaphore> vkSignalSmps(signalSemaphoreCount);
		for(i=0; i< signalSemaphoreCount; ++i) {
			vkSignalSmps[i] = reinterpret_cast<RSemaphoreVk*>(signalSemaphores + i)->handle;
		}
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vkSignalSmps.size());
		submitInfo.pSignalSemaphores = vkSignalSmps.data();

		// fence
		VkFence vkFence = (nullptr == fence) ? VK_NULL_HANDLE : reinterpret_cast<RFenceVk*>(fence)->handle;

		VK_CHECK(vkQueueSubmit(reinterpret_cast<RQueueVk*>(queue)->handle, 1, &submitInfo, vkFence), "vkQueueSubmit");
	}
	void RHIVulkan::QueueWaitIdle(RQueue* queue)
	{
		VK_CHECK(vkQueueWaitIdle(reinterpret_cast<RQueueVk*>(queue)->handle), "vkQueueWaitIdle");
	}

	RImageView* RHIVulkan::GetSwapchainImageView(uint8_t i)
	{
		ASSERT(i < m_MaxFramesInFlight, "i < m_MaxFramesInFlight");
		return reinterpret_cast<RImageView*>(&m_SwapchainImageViews[i]);
	}
	uint32_t RHIVulkan::GetSwapchainMaxImageCount()
	{
		return (uint32_t)m_SwapchainImages.size();
	}
	RFramebuffer* RHIVulkan::CreateFrameBuffer(RRenderPass* pass, const TVector<RImageView*>& imageViews, uint32_t width, uint32_t height, uint32_t layers)
	{
		VkFramebufferCreateInfo framebufferInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = reinterpret_cast<RRenderPassVk*>(pass)->handle;
		TVector<VkImageView> vkImageViews(imageViews.size());
		for(uint32_t i=0; i< vkImageViews.size(); ++i) {
			vkImageViews[i] = reinterpret_cast<RImageViewVk*>(imageViews[i])->handle;
		}
		framebufferInfo.attachmentCount = static_cast<uint32_t>(vkImageViews.size());
		framebufferInfo.pAttachments = vkImageViews.data();
		framebufferInfo.width = width;
		framebufferInfo.height = height;
		framebufferInfo.layers = layers;

		VkFramebuffer handle;
		if(VK_SUCCESS != vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &handle)) {
			return nullptr;
		}

		RFramebufferVk* framebuffer = new RFramebufferVk;
		framebuffer->handle = handle;
		return framebuffer;
	}

	void RHIVulkan::DestoryFramebuffer(RFramebuffer* framebuffer)
	{
		RFramebufferVk* framebufferVk = (RFramebufferVk*)framebuffer;
		vkDestroyFramebuffer(m_Device, framebufferVk->handle, nullptr);
		delete framebufferVk;
	}
	RCommandBuffer* RHIVulkan::CreateCommandBuffer(RCommandBufferLevel level)
	{
		VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.pNext = nullptr;
		allocateInfo.commandPool = m_RHICommandPool;
		allocateInfo.level = (VkCommandBufferLevel)level;
		allocateInfo.commandBufferCount = 1;
		VkCommandBuffer handle;
		if(VK_SUCCESS != vkAllocateCommandBuffers(m_Device, &allocateInfo, &handle)){
			return nullptr;
		}
		RCommandBufferVk* cmd = new RCommandBufferVk;
		cmd->handle = handle;
		cmd->m_Pool = allocateInfo.commandPool;
		return cmd;
	}
	void RHIVulkan::BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags)
	{
		RCommandBufferVk* vkCmd = static_cast<RCommandBufferVk*>(cmd);
		VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.pNext = nullptr;
		beginInfo.flags = flags;
		beginInfo.pInheritanceInfo = nullptr;
		VK_CHECK(_vkBeginCommandBuffer(vkCmd->handle, &beginInfo), "Failed to begin command buffer!");
	}
	void RHIVulkan::EndCommandBuffer(RCommandBuffer* cmd)
	{
		VK_CHECK(_vkEndCommandBuffer(static_cast<RCommandBufferVk*>(cmd)->handle), "Failed to end command buffer!");
	}
	void RHIVulkan::FreeCommandBuffer(RCommandBuffer* cmd)
	{
		RCommandBufferVk* vkCmd = reinterpret_cast<RCommandBufferVk*>(cmd);
		vkFreeCommandBuffers(m_Device, vkCmd->m_Pool, 1, &vkCmd->handle);
		delete vkCmd;
	}
	void RHIVulkan::CmdBeginRenderPass(RCommandBuffer* cmd, RRenderPass* pass, RFramebuffer* framebuffer, RSRect2D renderArea, uint32_t clearValueCount, const RSClearValue* clearValues)
	{
		VkRect2D vkRenderArea;
		memcpy(&vkRenderArea, &renderArea, sizeof(VkRect2D));
		VkRenderPassBeginInfo passInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		passInfo.pNext = nullptr;
		passInfo.renderPass = reinterpret_cast<RRenderPassVk*>(pass)->handle;
		passInfo.framebuffer = reinterpret_cast<RFramebufferVk*>(framebuffer)->handle;
		passInfo.renderArea = vkRenderArea;
		passInfo.clearValueCount = clearValueCount;
		passInfo.pClearValues = reinterpret_cast<const VkClearValue*>(clearValues);
		_vkCmdBeginRenderPass(((RCommandBufferVk*)cmd)->handle, &passInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	void RHIVulkan::CmdEndRenderPass(RCommandBuffer* cmd)
	{
		_vkCmdEndRenderPass(((RCommandBufferVk*)cmd)->handle);
	}
	void RHIVulkan::CmdTransitionImageLayout(RCommandBuffer* cmd, RImage* image, RImageLayout oldLayout, RImageLayout newLayout, uint32_t baseLevel, uint32_t levelCount, uint32_t baseLayer, uint32_t layerCount, RImageAspectFlags aspect)
	{
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = (VkImageLayout)oldLayout;
		barrier.newLayout = (VkImageLayout)oldLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = ((RImageVk*)image)->handle;
		barrier.subresourceRange.aspectMask = aspect;
		barrier.subresourceRange.baseMipLevel = baseLevel;
		barrier.subresourceRange.levelCount = levelCount;
		barrier.subresourceRange.baseArrayLayer = baseLayer;
		barrier.subresourceRange.layerCount = layerCount;
		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;
		GetPipelineBarrierStage(barrier.oldLayout, barrier.newLayout, barrier.srcAccessMask, barrier.dstAccessMask, srcStage, dstStage);
		vkCmdPipelineBarrier(((RCommandBufferVk*)cmd)->handle, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	}
	void RHIVulkan::CmdCopyBufferToImage(RCommandBuffer* cmd, RBuffer* buffer, RImage* image, RImageAspectFlags aspect, uint32_t mipLevel, uint32_t baseLayout, uint32_t layerCount)
	{
		RImageVk* imageVk = (RImageVk*)image;
		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = mipLevel;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { imageVk->m_Extent.width, imageVk->m_Extent.height, imageVk->m_Extent.depth};
		vkCmdCopyBufferToImage(((RCommandBufferVk*)cmd)->handle, ((RBufferVk*)buffer)->handle, imageVk->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	}
	void RHIVulkan::CmdBlitImage(RCommandBuffer* cmd, RImage* srcImage, RImage* dstImage, const RSImageBlit* pRegion)
	{
		VkImageBlit blit{};
		blit.srcSubresource.aspectMask = pRegion->srcAspect;
		blit.srcSubresource.baseArrayLayer = pRegion->srcBaseLayer;
		blit.srcSubresource.layerCount = pRegion->srcLayerCount;
		memcpy(blit.srcOffsets, pRegion->srcOffsets, sizeof(VkOffset3D) * 2);
		blit.dstSubresource.aspectMask = pRegion->dstAspect;
		blit.dstSubresource.baseArrayLayer = pRegion->dstBaseLayer;
		blit.dstSubresource.layerCount = pRegion->dstLayerCount;
		memcpy(blit.dstOffsets, pRegion->dstOffsets, sizeof(VkOffset3D) * 2);
		vkCmdBlitImage(((RCommandBufferVk*)cmd)->handle, ((RImageVk*)srcImage)->handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			((RImageVk*)dstImage)->handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
	}
	void RHIVulkan::CmdGenerateMipMap(RCommandBuffer* cmd, RImage* image, uint32_t levelCount, RImageAspectFlags aspect, uint32_t baseLayer, uint32_t layerCount)
	{
		RImageVk* imageVk = (RImageVk*)image;
		GenerateMipMap(((RCommandBufferVk*)cmd)->handle, imageVk->handle, levelCount, imageVk->m_Extent.width, imageVk->m_Extent.height, aspect, baseLayer, layerCount);
	}
	void RHIVulkan::ImmediateCommit(CommandBufferFunc func)
	{
		RCommandBufferVk cmd;

		// allocate
		VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.pNext = nullptr;
		allocateInfo.commandPool = m_RHICommandPool;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandBufferCount = 1;
		VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocateInfo, &cmd.handle), "Failed to allocate command buffers!");
		cmd.m_Pool = m_RHICommandPool;

		// begin
		VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.pNext = nullptr;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;
		VK_CHECK(_vkBeginCommandBuffer(cmd.handle, &beginInfo), "Failed to begin command buffer!");

		// run
		func(&cmd);

		// end
		_vkEndCommandBuffer(cmd.handle);

		// submit
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmd.handle;
		vkQueueSubmit(m_GraphicsQueue.handle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue.handle);

		// free
		vkFreeCommandBuffers(m_Device, cmd.m_Pool, 1, &cmd.handle);
	}

	uint32_t RHIVulkan::PrepareRendering()
	{
		vkWaitForFences(m_Device, 1, &m_IsFrameInFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);
		VkResult res = 
			vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_ImageAvaliableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &m_CurrentSwapchainImageIndex);
		// todo acquire next image khr result

		VK_CHECK(_vkResetCommandPool(m_Device, m_CommandPools[m_CurrentFrame], 0), "Failed to reset command pool!");
		return m_CurrentSwapchainImageIndex;
	}
	void RHIVulkan::QueueSubmitRendering(RCommandBuffer* cmd)
	{
		VK_CHECK(_vkResetFences(m_Device, 1, &m_IsFrameInFlightFences[m_CurrentFrame]));

		// submit command buffer
		//VkSemaphore semaphores[2] = { m_ImageAvaliableForTextureCopySemaphores[m_CurrentFrame], m_PresentationFinishSemaphores[m_CurrentFrame] };
		VkSemaphore semaphores[1] = { m_PresentationFinishSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		VkCommandBuffer vkCmd = reinterpret_cast<RCommandBufferVk*>(cmd)->handle;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_ImageAvaliableSemaphores[m_CurrentFrame];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vkCmd;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = semaphores;
		VK_CHECK(vkQueueSubmit(m_GraphicsQueue.handle, 1, &submitInfo, m_IsFrameInFlightFences[m_CurrentFrame]), "Failed to submit graphics queue!");

		// present
		VkPresentInfoKHR presentQueue = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentQueue.waitSemaphoreCount = 1;
		presentQueue.pWaitSemaphores = &m_PresentationFinishSemaphores[m_CurrentFrame];
		presentQueue.swapchainCount = 1;
		presentQueue.pSwapchains = &m_Swapchain;
		presentQueue.pImageIndices = &m_CurrentSwapchainImageIndex;

		VkResult presentResult = vkQueuePresentKHR(m_PresentQueue.handle, &presentQueue);
		// todo present result

		m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;

	}
	void RHIVulkan::DestroyMemory(RMemory* memory)
	{
		vmaFreeMemory(m_Vma, reinterpret_cast<RMemoryVma*>(memory)->handle);
		delete memory;
	}

	RBuffer* RHIVulkan::CreatBuffer(size_t size, RBufferUsage usage)
	{
		VkBufferCreateInfo bufferCreateInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // not sharing among queue families

		VkBuffer handle;
		if (VK_SUCCESS != vkCreateBuffer(m_Device, &bufferCreateInfo, nullptr, &handle)) {
			return nullptr;
		}
		RBufferVk* buffer = new RBufferVk;
		buffer->handle = handle;
		return buffer;
	}

	void RHIVulkan::CreateBufferWithMemory(size_t size, RBufferUsage usage, RMemoryPropertyFlags memoryFlags, RBuffer*& pBuffer, RMemory*& pMemory, size_t dataSize, void* pData)
	{
		VkBufferCreateInfo bufferCreateInfo{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // not sharing among queue families

		VmaAllocationCreateInfo allocInfo;
		allocInfo.flags = 0;
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = (VkMemoryPropertyFlags)memoryFlags;
		allocInfo.preferredFlags = (VkMemoryPropertyFlags)memoryFlags;
		allocInfo.memoryTypeBits = 0;
		allocInfo.pool = nullptr;
		allocInfo.pUserData = pData;
		allocInfo.priority = 1.0f;

		VkBuffer bufferHandle;
		VmaAllocation allocHandle;
		if(VK_SUCCESS != vmaCreateBuffer(m_Vma, &bufferCreateInfo, &allocInfo, &bufferHandle, &allocHandle, nullptr)) {
			return;
		}

		RBufferVk* bufferVk = new RBufferVk;
		bufferVk->handle = bufferHandle;
		RMemoryVma* memoryVk = new RMemoryVma;
		memoryVk->handle = allocHandle;

		pBuffer = bufferVk;
		pMemory = memoryVk;
	}

	void RHIVulkan::DestroyBuffer(RBuffer* buffer)
	{
		RBufferVk* bufferVk = (RBufferVk*)buffer;
		vkDestroyBuffer(m_Device, bufferVk->handle, nullptr);
		delete bufferVk;
	}

	RImageVk* RHIVulkan::CreateImage(RImageType type, RFormat format, RSExtent3D&& extent, uint32_t mipLevels, uint32_t arrayLayers, RSampleCountFlags samples, RImageTiling tiling, RImageUsageFlags usage)
	{
		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.pNext = nullptr;
		imageInfo.flags = 0;
		imageInfo.imageType = (VkImageType)type;
		imageInfo.format = (VkFormat)format;
		imageInfo.extent.width = extent.width; imageInfo.extent.height = extent.height; imageInfo.extent.depth = extent.depth;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = arrayLayers;
		imageInfo.samples = (VkSampleCountFlagBits)samples;
		imageInfo.tiling = (VkImageTiling)tiling;
		imageInfo.usage = (VkImageUsageFlags)usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VkImage handle;
		if(VK_SUCCESS != vkCreateImage(m_Device, &imageInfo, nullptr, &handle)) {
			return nullptr;
		}
		RImageVk* image = new RImageVk();
		image->handle = handle;
		image->m_Type = type;
		image->m_Extent = extent;
		image->m_Format = format;
		return image;
	}

	RImage* RHIVulkan::CreateImage2D(RFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, RSampleCountFlagBits samples, RImageTiling tiling, RImageUsageFlags usage)
	{
		return CreateImage(IMAGE_TYPE_2D, format, {width, height, 0}, mipLevels, 1, samples, tiling, usage);
	}

	RMemory* RHIVulkan::CreateImageMemory(RImage* image, RMemoryPropertyFlags memoryProperty, void* pData)
	{
		VkImage imageVk = reinterpret_cast<RImageVk*>(image)->handle;
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_Device, imageVk, &memRequirements);
		RMemoryVma* memory = new RMemoryVma;

		VmaAllocationCreateInfo info;
		info.flags = 0;
		info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		info.requiredFlags = (VkMemoryPropertyFlags)memoryProperty;
		info.preferredFlags = (VkMemoryPropertyFlags)memoryProperty;
		info.memoryTypeBits = 0;
		info.pool = nullptr;
		info.pUserData = pData;
		info.priority = 1.0f;
		vmaAllocateMemoryForImage(m_Vma, imageVk, &info, &memory->handle, nullptr);
		vmaBindImageMemory(m_Vma, memory->handle, imageVk);
		return reinterpret_cast<RMemory*>(memory);
	}

	void RHIVulkan::DestroyImage(RImage* image)
	{
		RImageVk* imageVk = (RImageVk*)image;
		vkDestroyImage(m_Device, imageVk->handle, nullptr);
		delete imageVk;
	}

	RImageView* RHIVulkan::CreateImageView(RImage* image, RImageViewType viewType, RImageAspectFlags aspectMask,
		uint32_t baseMipLevel, uint32_t levelCount, uint32_t baseLayer, uint32_t layerCount)
	{
		RImageVk* imageVk = (RImageVk*)image;
		VkImageViewCreateInfo imageViewCreateInfo{};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = imageVk->handle;
		imageViewCreateInfo.viewType = (VkImageViewType)viewType;
		imageViewCreateInfo.format = (VkFormat)imageVk->m_Format;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
		imageViewCreateInfo.subresourceRange.baseMipLevel = baseMipLevel;
		imageViewCreateInfo.subresourceRange.levelCount = levelCount;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = baseLayer;
		imageViewCreateInfo.subresourceRange.layerCount = layerCount;
		VkImageView handle;
		if (VK_SUCCESS != vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &handle)) {
			return nullptr;
		}
		RImageViewVk* imageView = new RImageViewVk;
		imageView->handle = handle;
		return imageView;
	}

	void RHIVulkan::DestroyImageView(RImageView* imageView)
	{
		RImageViewVk* imageViewVk = (RImageViewVk*)imageView;
		vkDestroyImageView(m_Device, imageViewVk->handle, nullptr);
		delete imageViewVk;
	}

	RSampler* RHIVulkan::CreateSampler(const RSSamplerInfo* samplerInfo)
	{
		VkSamplerCreateInfo info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		info.pNext = nullptr;
		info.flags = 0;
		info.magFilter = (VkFilter)samplerInfo->magFilter;
		info.minFilter = (VkFilter)samplerInfo->minFilter;
		info.addressModeU = (VkSamplerAddressMode)samplerInfo->addressModeU;
		info.addressModeV = (VkSamplerAddressMode)samplerInfo->addressModeV;
		info.addressModeW = (VkSamplerAddressMode)samplerInfo->addressModeW;
		info.mipLodBias = samplerInfo->minLodBias;
		info.anisotropyEnable = samplerInfo->anisotropyEnable;
		info.maxAnisotropy = samplerInfo->maxAnisotropy;
		info.compareEnable = samplerInfo->compareEnable;
		info.compareOp = (VkCompareOp)samplerInfo->compreOp;
		info.minLod = samplerInfo->minLod;
		info.maxLod = samplerInfo->maxLod;
		info.borderColor = (VkBorderColor)samplerInfo->borderColor;
		info.unnormalizedCoordinates = samplerInfo->unnormalizedCoordinates;

		VkSampler handle;
		if(VK_SUCCESS != vkCreateSampler(m_Device, &info, nullptr, &handle)) {
			return nullptr;
		}
		RSamplerVk* sampler = new RSamplerVk;
		sampler->handle = handle;
		return sampler;
	}

} // namespace RHI
