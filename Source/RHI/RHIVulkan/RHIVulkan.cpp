#include "RHIVulkan.h"

#include <GLFW/glfw3.h>

#include "Core/macro.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace RHI {

#define VK_CHECK(x, s)\
	if(VK_SUCCESS != x) throw s

	const uint32_t VK_API_VER = VK_API_VERSION_1_2;

#pragma region private members
	// debug callback
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* data)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		RHIVulkan* rhi = reinterpret_cast<RHIVulkan*>(data);
		rhi->m_IsNormal = false;
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
		ASSERT(m_EnableValidationLayers && !CheckValidationLayerSupport(), "validation layers requested, but not available!");
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
		LOG("count of GPU = %d", deviceCount);
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

		LOG_ERROR("findSupportedFormat failed");
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
		vkGetDeviceQueue(m_Device, static_cast<uint32_t>(m_PhysicalDeviceInfo.graphicsIndex), 0, &m_GraphicsQueue.m_VkQueue);
		vkGetDeviceQueue(m_Device, static_cast<uint32_t>(m_PhysicalDeviceInfo.computeIndex), 0, &m_ComputeQueue.m_VkQueue);
		vkGetDeviceQueue(m_Device, static_cast<uint32_t>(m_PhysicalDeviceInfo.presentIndex), 0, &m_PresentQueue.m_VkQueue);

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
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			commandPoolCreateInfo.queueFamilyIndex = static_cast<uint32_t>(m_PhysicalDeviceInfo.graphicsIndex);
			for (uint32_t i = 0; i < s_MaxFramesInFlight; ++i) {
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
		for (uint32_t i = 0; i < s_MaxFramesInFlight; ++i)
		{
			commandBufferAllocateInfo.commandPool = m_CommandPools[i];
			VK_CHECK(vkAllocateCommandBuffers(m_Device, &commandBufferAllocateInfo, &m_CommandBuffers[i].m_VkCmd), "vkAllocateCommandBuffers");
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

		for(uint32_t i=0; i< s_MaxFramesInFlight; ++i) {
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
		vmaCreateAllocator(&allocatorCreateInfo, &m_VMA);
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
		m_Scissor = { {0, 0,}, {m_PhysicalDeviceInfo.swapchainExtent.width, m_PhysicalDeviceInfo.swapchainExtent.height} };

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
			VK_CHECK(vkCreateImageView(m_Device, &imageViewCreateInfo, nullptr, &m_SwapchainImageViews[i]), "CreateImageView");
		}
	}
	void RHIVulkan::ClearSwapchain()
	{
		for(auto imageView: m_SwapchainImageViews) {
			vkDestroyImageView(m_Device, imageView, nullptr);
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
	}

	void RHIVulkan::Release()
	{
		//vma
		vmaDestroyAllocator(m_VMA);
		m_VMA = nullptr;
		//sync
		for (auto smp : m_ImageAvaliableSemaphores) {
			vkDestroySemaphore(m_Device, smp, nullptr);
		}
		for (auto smp : m_PresentationFinishSemaphores) {
			vkDestroySemaphore(m_Device, smp, nullptr);
		}
		for (auto smp : m_ImageAvaliableForTextureCopySemaphores) {
			vkDestroySemaphore(m_Device, smp, nullptr);
		}
		for (auto fence : m_IsFrameInFlightFences) {
			vkDestroyFence(m_Device, fence, nullptr);
		}
		for (auto pool : m_CommandPools) {
			vkDestroyCommandPool(m_Device, pool, nullptr);
		}
		vkDestroyCommandPool(m_Device, m_RHICommandPool, nullptr);
		vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
		for (auto imageView : m_SwapchainImageViews) {
			vkDestroyImageView(m_Device, imageView, nullptr);
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
		info.queue = m_GraphicsQueue.m_VkQueue;
		info.descriptorPool = m_DescriptorPool;
		return info;
	}

	RCommandBuffer* RHIVulkan::GetCurrentCommandBuffer()
	{
		return static_cast<RCommandBuffer*>(&m_CommandBuffers[m_CurrentFrame]);
	}
	void RHIVulkan::CreateRenderPass(RRenderPass* pass, uint32_t attachmentCount, RSAttachment* attachments)
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

		RVkRenderPass* vkPass = static_cast<RVkRenderPass*>(pass);
		vkCreateRenderPass(m_Device, &info, nullptr, &vkPass->m_VkRenderPass);		
	}
	void RHIVulkan::DestroyRenderPass(RRenderPass* pass)
	{
		RVkRenderPass* vkPass = static_cast<RVkRenderPass*>(pass);
		if(nullptr != vkPass->m_VkRenderPass) {
			vkDestroyRenderPass(m_Device, vkPass->m_VkRenderPass, nullptr);
			vkPass->m_VkRenderPass = nullptr;
		}
	}
	RCommandBuffer* RHIVulkan::CreateCommandBuffer(RCommandBufferLevel level)
	{
		RVkCommandBuffer* cmd = new RVkCommandBuffer();
		VkCommandBufferAllocateInfo allocateInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocateInfo.pNext = nullptr;
		allocateInfo.commandPool = m_RHICommandPool;
		allocateInfo.level = (VkCommandBufferLevel)level;
		allocateInfo.commandBufferCount = 1;
		VK_CHECK(vkAllocateCommandBuffers(m_Device, &allocateInfo, &cmd->m_VkCmd), "Failed to allocate command buffers!");
		return cmd;
	}
	void RHIVulkan::BeginCommandBuffer(RCommandBuffer* cmd, RCommandBufferUsageFlags flags)
	{
		RVkCommandBuffer* vkCmd = static_cast<RVkCommandBuffer*>(cmd);
		VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		beginInfo.pNext = nullptr;
		beginInfo.flags = flags;
		beginInfo.pInheritanceInfo = nullptr;
		VK_CHECK(vkBeginCommandBuffer(vkCmd->m_VkCmd, &beginInfo), "Failed to begin command buffer!");
	}
	void RHIVulkan::EndCommandBuffer(RCommandBuffer* cmd)
	{
		VK_CHECK(vkEndCommandBuffer(static_cast<RVkCommandBuffer*>(cmd)->m_VkCmd), "Failed to end command buffer!");
	}
	void RHIVulkan::FreeCommandBuffer(RCommandBuffer*& cmd)
	{
		RVkCommandBuffer* vkCmd = reinterpret_cast<RVkCommandBuffer*>(cmd);
		vkFreeCommandBuffers(m_Device, m_RHICommandPool, 1, &vkCmd->m_VkCmd);
		delete cmd;
		cmd = nullptr;
	}
	RQueue* RHIVulkan::GetGraphicsQueue()
	{
		return reinterpret_cast<RQueue*>(&m_GraphicsQueue);
	}
	void RHIVulkan::QueueSubmit(RQueue* queue, TVector<RCommandBuffer*> cmds, TVector<TPair<RSemaphore*, RPipelineStageFlags>> waitSemaphores, TVector<RSemaphore*> signalSemaphores, RFence* fence)
	{
		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.pNext = nullptr;
		uint32_t i = 0;

		TVector<VkSemaphore> vkWaitSmps(waitSemaphores.size());
		TVector<VkPipelineStageFlags> stageFlags(waitSemaphores.size());
		for (i = 0; i < waitSemaphores.size(); ++i) {
			vkWaitSmps[i] = reinterpret_cast<RVkSemaphore*>(waitSemaphores[i].first)->m_VkSemaphore;
			stageFlags[i] = static_cast<VkPipelineStageFlags>(waitSemaphores[i].second);
		}
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(vkWaitSmps.size());
		submitInfo.pWaitSemaphores = vkWaitSmps.data();
		submitInfo.pWaitDstStageMask = stageFlags.data();
		TVector<VkCommandBuffer> vkCmds(cmds.size());
		for (i = 0; i < cmds.size(); ++i) {
			vkCmds[i] = reinterpret_cast<RVkCommandBuffer*>(cmds[i])->m_VkCmd;
		}
		TVector<VkSemaphore> vkSignalSmps(signalSemaphores.size());
		for(i=0; i< signalSemaphores.size(); ++i) {
			vkSignalSmps[i] = reinterpret_cast<RVkSemaphore*>(signalSemaphores[i])->m_VkSemaphore;\
		}
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(vkSignalSmps.size());
		submitInfo.pSignalSemaphores = vkSignalSmps.data();

		VkFence vkFence = nullptr == fence ? VK_NULL_HANDLE : reinterpret_cast<RVkFence*>(fence)->m_VkFence;

		VK_CHECK(vkQueueSubmit(reinterpret_cast<RVkQueue*>(queue)->m_VkQueue, 1, &submitInfo, vkFence), "vkQueueSubmit");
	}
	void RHIVulkan::QueueWaitIdle(RQueue* queue)
	{
		VK_CHECK(vkQueueWaitIdle(reinterpret_cast<RVkQueue*>(queue)->m_VkQueue), "vkQueueWaitIdle");
	}
} // namespace RHI
