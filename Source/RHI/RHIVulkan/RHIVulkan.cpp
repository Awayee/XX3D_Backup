#include "RHIVulkan.h"
#include "VulkanUtil.h"
#include <GLFW/glfw3.h>
#ifdef USE_VMA
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>
#else
#include <vulkan/vulkan.h>
#endif
#include "Core/Memory/SmartPointer.h"

namespace RHI {

#define RETURN_RHI_PTR(cls, hd)\
	cls##Vk* cls##Ptr = new cls##Vk;\
	cls##Ptr->handle = hd; \
	return reinterpret_cast<cls*>(cls##Ptr)

#pragma region rhi initialzie

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
		ASSERT(!m_EnableValidationLayers || CheckValidationLayerSupport(m_ValidationLayers), 
			"validation layers requested, but not available!");
		// app info
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = m_AppName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = m_AppName.c_str();
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

	void RHIVulkan::InitializePhysicalDevice()
	{
		uint32_t deviceCount = 0;
		VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr), "no avaliable physical device");
		TVector<VkPhysicalDevice> devices(deviceCount);
		VK_CHECK(vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data()), "no avaliable physical device");
		TVector<TPair<VkPhysicalDevice, SPhyicalDeviceInfo>> devicesInfo;
		for (auto& device : devices) {
			auto deviceInfo = GetPhysicalDeviceInfo(device, m_Surface, m_DeviceExtensions);
			if(deviceInfo.score > 0) {
				devicesInfo.push_back({device, std::move(deviceInfo)});
			}
		}

		ASSERT(!devicesInfo.empty(), "no avaliable GPU!");

		TSort(devicesInfo.begin(), devicesInfo.end(),
			[](const TPair<VkPhysicalDevice, SPhyicalDeviceInfo> p1, const TPair<VkPhysicalDevice, SPhyicalDeviceInfo> p2) {return p1.second.score > p2.second.score; });

		m_PhysicalDevice = devicesInfo[0].first;
		auto& deviceInfo = devicesInfo[0].second;
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_PhysicalDeviceProperties);
		LOG("Picked GPU: %s %u", m_PhysicalDeviceProperties.deviceName, m_PhysicalDeviceProperties.deviceID);

		// record device info
		m_GraphicsIndex = static_cast<uint32_t>(deviceInfo.graphicsIndex);
		m_PresentIndex = static_cast<uint32_t>(deviceInfo.presentIndex);
		m_ComputeIndex = static_cast<uint32_t>(deviceInfo.computeIndex);
		m_ImageCount = deviceInfo.imageCount;
		m_SwapchainFormat = deviceInfo.swapchainFormat;
		m_SwapchainPresentMode = deviceInfo.swapchainPresentMode;
		m_SwapchainTransform = deviceInfo.swapchainTransform;
		m_SwapchainExtent.width = deviceInfo.swapchainExtent.width;
		m_SwapchainExtent.height = deviceInfo.swapchainExtent.height;
	}

	void RHIVulkan::CreateLogicalDevice()
	{
		TUnorderedSet<uint32_t> queueFamilies{m_GraphicsIndex, m_PresentIndex, m_ComputeIndex};
		TVector<VkDeviceQueueCreateInfo> queueCreateInfos;
		queueCreateInfos.reserve(queueFamilies.size());
		float queuePriority = 1.0f;
		for(uint32_t queueFamily: queueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(std::move(queueCreateInfo));
		}

		VkPhysicalDeviceFeatures features{};
		features.samplerAnisotropy = VK_TRUE;
		features.fragmentStoresAndAtomics = VK_TRUE;
		features.independentBlend = VK_TRUE;
		features.geometryShader = m_EnableGeometryShader;

		VkDeviceCreateInfo deviceCreateInfo{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &features;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();
		deviceCreateInfo.enabledLayerCount = 0;

		VK_CHECK(vkCreateDevice(m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device), "vkCreateDevice");

		// initialize queues of this device
		vkGetDeviceQueue(m_Device, m_GraphicsIndex, 0, &m_GraphicsQueue.handle);
		vkGetDeviceQueue(m_Device, m_ComputeIndex, 0, &m_ComputeQueue.handle);
		vkGetDeviceQueue(m_Device, m_PresentIndex, 0, &m_PresentQueue.handle);

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
		_vkCmdDraw = (PFN_vkCmdDraw)vkGetDeviceProcAddr(m_Device, "vkCmdDraw");
		_vkCmdDispatch = (PFN_vkCmdDispatch)vkGetDeviceProcAddr(m_Device, "vkCmdDispatch");
		_vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)vkGetDeviceProcAddr(m_Device, "vkCmdCopyBuffer");

		m_DepthFormat = FindDepthFormat(m_PhysicalDevice);
	}

	void RHIVulkan::CreateCommandPools()
	{
		// graphics command pool
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo{};
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = NULL;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = m_GraphicsIndex;
			VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_RHICommandPool), "VkCreateCommandPool");
		}

		// other command pool
		{
			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = NULL;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			commandPoolCreateInfo.queueFamilyIndex = m_GraphicsIndex;
			m_CommandPools.resize(m_MaxFramesInFlight);
			for (uint32_t i = 0; i < m_MaxFramesInFlight; ++i) {
				VK_CHECK(vkCreateCommandPool(m_Device, &commandPoolCreateInfo, nullptr, &m_CommandPools[i]), "VkCreateCommandPool");
			}
		}
	}

	void RHIVulkan::CreateDescriptorPool()
	{
		// Since DescriptorSet should be treated as asset in Vulkan, DescriptorPool
		// should be big enough, and thus we can sub-allocate DescriptorSet from
		// DescriptorPool merely as we sub-allocate Buffer/Image from DeviceMemory.

		uint32_t maxVertexBlendingMeshCount{ 256 };
		uint32_t maxMaterialCount{ 256 };

		uint32_t poolCount = 7;
		TArray<VkDescriptorPoolSize> poolSizes(poolCount);
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
		poolInfo.poolSizeCount = poolCount;
		poolInfo.pPoolSizes = poolSizes.Data();
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
		createInfo.minImageCount = m_ImageCount;
		createInfo.imageFormat = m_SwapchainFormat.format;
		createInfo.imageColorSpace = m_SwapchainFormat.colorSpace;
		createInfo.imageExtent.width = m_SwapchainExtent.width;
		createInfo.imageExtent.height = m_SwapchainExtent.height;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		uint32_t queueFamilyIndices[] = { m_GraphicsIndex, m_PresentIndex};

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
		createInfo.preTransform = m_SwapchainTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_SwapchainPresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		VK_CHECK(vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_Swapchain), "CreateSwapchain");
		// swapchain images
		uint32_t image_count;
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &image_count, nullptr);
		m_SwapchainImages.resize(image_count);
		vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &image_count, m_SwapchainImages.data());
		m_Scissor = { {0, 0,}, {m_SwapchainExtent.width, m_SwapchainExtent.height} };
		// Create image views
		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		// create imageview (one for each this time) for all swapchain images
		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imageViewCreateInfo.image = m_SwapchainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_SwapchainFormat.format;
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
		CreateDescriptorPool();
		CreateSyncResources();
		CreateSwapchain();
		CreateMemoryAllocator();

		m_Initialized = true;

		LOG("RHI: Vulkan initialized successfully!");
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
		info.queueIndex = m_GraphicsIndex;
		info.queue = m_GraphicsQueue.handle;
		info.descriptorPool = m_DescriptorPool;
		return info;
	}

#pragma endregion
	RRenderPass* RHIVulkan::CreateRenderPass(uint32_t attachmentCount, const RSAttachment* attachments)
	{
		VkRenderPassCreateInfo info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr, 0 };
		info.attachmentCount = attachmentCount;
		TArray<VkAttachmentDescription> descs(attachmentCount);
		TVector<VkAttachmentReference> colorRefs;
		TUniquePtr<VkAttachmentReference> depthRefPtr;
		for(uint32_t i=0; i<attachmentCount; ++i) {
			descs[i].flags = 0;
			descs[i].format = (VkFormat)attachments[i].format;
			descs[i].samples = (VkSampleCountFlagBits)attachments[i].sampleCount;
			descs[i].loadOp = (VkAttachmentLoadOp)attachments[i].loadOp;
			descs[i].storeOp = (VkAttachmentStoreOp)attachments[i].storeOp;
			descs[i].stencilLoadOp = (VkAttachmentLoadOp)attachments[i].stencilLoadOp;
			descs[i].stencilStoreOp = (VkAttachmentStoreOp)attachments[i].stencilStoreOp;
			descs[i].initialLayout = (VkImageLayout)attachments[i].initialLayout;
			descs[i].finalLayout = (VkImageLayout)attachments[i].finalLayout;
			if(attachments[i].type == ATTACHMENT_COLOR) {
				colorRefs.push_back({i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
			}
			else if (attachments[i].type == ATTACHMENT_DEPTH) {
				depthRefPtr.reset(new VkAttachmentReference{i, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL });
			}
		}
		info.pAttachments = descs.Data();

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
	RRenderPass* RHIVulkan::CreateRenderPass(const RRenderPassData& data)
	{
		uint32_t i;
		TVector<VkAttachmentDescription> attachments;

		// sub passes
		TArray<VkSubpassDescription> subPasses(data.SubPasses.size());
		TArray<TVector<VkAttachmentReference>> inputAttachments(data.SubPasses.size());
		TArray<TVector<VkAttachmentReference>> colorAttachments(data.SubPasses.size());
		TArray<VkAttachmentReference> depthAttachments(data.SubPasses.size());

		for(i=0; i<data.SubPasses.size(); ++i) {
			subPasses[i].pipelineBindPoint = (VkPipelineBindPoint)data.SubPasses[i].Type;
			uint32_t j;
			for(j=0; j<data.SubPasses[i].InputAttachments.size(); ++j) {
				VkAttachmentDescription desc = ResolveAttachmentDesc(data.SubPasses[i].InputAttachments[j]);
				inputAttachments[i].push_back({ (uint32_t)(attachments.size()), desc.finalLayout });
				attachments.push_back(std::move(desc));
			}
			subPasses[i].inputAttachmentCount = data.SubPasses[i].InputAttachments.size();
			subPasses[i].pInputAttachments = inputAttachments[i].data();

			for(j=0; j<data.SubPasses[i].ColorAttachments.size(); ++j) {
				VkAttachmentDescription desc = ResolveAttachmentDesc(data.SubPasses[i].InputAttachments[j]);
				colorAttachments[i].push_back({ (uint32_t)(attachments.size()), desc.finalLayout });
				attachments.push_back(std::move(desc));
			}
			subPasses[i].colorAttachmentCount = data.SubPasses[i].ColorAttachments.size();
			subPasses[i].pColorAttachments = colorAttachments[i].data();

			if(!data.SubPasses[i].DepthStencilAttachments.empty()) {
				auto desc = ResolveAttachmentDesc(data.SubPasses[i].DepthStencilAttachments[0]);
				depthAttachments[i] = { (uint32_t)(attachments.size()), desc.finalLayout };
				attachments.push_back(std::move(desc));
			}
		}

		// sub pass dependencies
		TArray<VkSubpassDependency> dependencies(data.Dependencies.size());
		for(i=0; i<data.Dependencies.size(); ++i) {
			dependencies[i] = ResolveSubpassDependency(data.Dependencies[i]);
		}

		VkRenderPassCreateInfo info{ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO, nullptr };
		info.attachmentCount = attachments.size();
		info.pAttachments = attachments.data();
		info.subpassCount = data.SubPasses.size();
		info.pSubpasses = subPasses.Data();
		info.dependencyCount = data.Dependencies.size();
		info.pDependencies = dependencies.Data();
		VkRenderPass handle;
		if(VK_SUCCESS != vkCreateRenderPass(m_Device, &info, nullptr, &handle)) {
			return nullptr;
		}
		RRenderPassVk* renderPass = new RRenderPassVk;
		renderPass->handle = handle;
		return renderPass;
	}
	void RHIVulkan::DestroyRenderPass(RRenderPass* pass)
	{
		RRenderPassVk* vkPass = static_cast<RRenderPassVk*>(pass);
		vkDestroyRenderPass(m_Device, vkPass->handle, nullptr);
		delete vkPass;
	}
	RDescriptorSetLayout* RHIVulkan::CreateDescriptorSetLayout(uint32_t bindingCount, const RSDescriptorSetLayoutBinding* bindings)
	{
		VkDescriptorSetLayoutCreateInfo info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		info.pNext = nullptr;
		info.flags = 0;
		info.bindingCount = bindingCount;
		TArray<VkDescriptorSetLayoutBinding> bindingsVk(bindingCount);
		for(uint32_t i=0; i< bindingCount; ++i) {
			bindingsVk[i].binding = bindings[i].binding;
			bindingsVk[i].descriptorType = (VkDescriptorType)bindings[i].descriptorType;
			bindingsVk[i].descriptorCount = bindings[i].descriptorCount;
			bindingsVk[i].stageFlags = (VkShaderStageFlags)bindings[i].stageFlags;
			bindingsVk[i].pImmutableSamplers = nullptr;
		}
		info.pBindings = bindingsVk.Data();
		VkDescriptorSetLayout handle;
		if(VK_SUCCESS != vkCreateDescriptorSetLayout(m_Device, &info, nullptr, &handle)) {
			return nullptr;
		}
		RDescriptorSetLayoutVk* descriptorSetLayout = new RDescriptorSetLayoutVk;
		descriptorSetLayout->handle = handle;
		return descriptorSetLayout;

	}
	RDescriptorSet* RHIVulkan::AllocateDescriptorSet(const RDescriptorSetLayout* layout)
	{
		VkDescriptorSetAllocateInfo info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		info.pNext = nullptr;
		info.descriptorPool = m_DescriptorPool;
		info.descriptorSetCount = 1;
		info.pSetLayouts = &((RDescriptorSetLayoutVk*)layout)->handle;
		VkDescriptorSet handle;
		if (VK_SUCCESS != vkAllocateDescriptorSets(m_Device, &info, &handle)) {
			return nullptr;
		}
		RDescriptorSetVk* descriptorSet = new RDescriptorSetVk;
		descriptorSet->handle = handle;
		return descriptorSet;
	}

	void RHIVulkan::FreeDescriptorSet(RDescriptorSet* descriptorSet)
	{
		RDescriptorSetVk* descirptorSetVk = (RDescriptorSetVk*)descriptorSet;
		vkFreeDescriptorSets(m_Device, m_DescriptorPool, 1, &descirptorSetVk->handle);
		delete descirptorSetVk;
	}

	void RHIVulkan::UpdateDescriptorSet(RDescriptorSet* descriptorSet, uint32_t binding, uint32_t arrayElement, uint32_t count, RDescriptorType type, const RDescriptorInfo& descriptorInfo)
	{
		VkDescriptorSet handle = ((RDescriptorSetVk*)descriptorSet)->handle;
		VkWriteDescriptorSet write{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr};
		write.dstSet = handle;
		write.dstBinding = binding;
		write.dstArrayElement = arrayElement;
		write.descriptorCount = count;
		write.descriptorType = (VkDescriptorType)type;
		switch(type) {
			// buffer
		case (DESCRIPTOR_TYPE_UNIFORM_BUFFER):
		case (DESCRIPTOR_TYPE_STORAGE_BUFFER):
		case (DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC):
		case (DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC): {
			VkDescriptorBufferInfo bufferInfo{
				((RBufferVk*)descriptorInfo.buffer)->handle,
				descriptorInfo.offset, descriptorInfo.range };
			write.pBufferInfo = &bufferInfo;
			vkUpdateDescriptorSets(m_Device, 1, &write, 0, nullptr);
			break;
		}

			// image
		case (DESCRIPTOR_TYPE_SAMPLER):
		case (DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER):
		case (DESCRIPTOR_TYPE_SAMPLED_IMAGE):
		case (DESCRIPTOR_TYPE_STORAGE_IMAGE): {
			VkDescriptorImageInfo imageInfo{};
			if (nullptr != descriptorInfo.sampler) {
				imageInfo.sampler = ((RSamplerVk*)descriptorInfo.sampler)->handle;
			}
			if (nullptr != descriptorInfo.imageView) {
				RImageViewVk* imageViewVk = (RImageViewVk*)descriptorInfo.imageView;
				imageInfo.imageView = imageViewVk->handle;
				imageInfo.imageLayout = (VkImageLayout)imageViewVk->GetLayout();
			}
			write.pImageInfo = &imageInfo;
			vkUpdateDescriptorSets(m_Device, 1, &write, 0, nullptr);
			break;
		}

			// buffer view TODO
		case (DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER):
		case (DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER):{ }
		default: break;
		}
	}

	/*
	void RHIVulkan::AllocateDescriptorSets(uint32_t count, const RDescriptorSetLayout* const* layouts, RDescriptorSet* const* descriptorSets)
	{
		TArray<VkDescriptorSetLayout> layoutsVk(count);
		for(uint32_t i=0; i< count; ++i) {
			layoutsVk[i] = ((RDescriptorSetLayoutVk*)layouts[i])->handle;
		}
		VkDescriptorSetAllocateInfo info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
		info.pNext = nullptr;
		info.descriptorPool = m_DescriptorPool;
		info.descriptorSetCount = count;
		info.pSetLayouts = layoutsVk.Data();
		TArray<VkDescriptorSet> descriptorSetsVk(count);
		if(VK_SUCCESS != vkAllocateDescriptorSets(m_Device, &info, descriptorSetsVk.Data())) {
			return;
		}
		for(uint32_t i=0; i< count; ++i) {
			RDescriptorSetVk* descriptorSetVk = (RDescriptorSetVk*)descriptorSets[i];
			descriptorSetVk->handle = descriptorSetsVk[i];
		}
	}
	void RHIVulkan::FreeDescriptorSets(uint32_t count, RDescriptorSet** descriptorSets)
	{
		TArray<VkDescriptorSet> descriptorSetsVk(count);
		for(uint32_t i=0; i<count; ++i) {
			descriptorSetsVk[i] = ((RDescriptorSetVk*)descriptorSets[i])->handle;
			delete descriptorSets[i];
		}
		vkFreeDescriptorSets(m_Device, m_DescriptorPool, count, descriptorSetsVk.Data());
	}
	*/
	
	RPipelineLayout* RHIVulkan::CreatePipelineLayout(uint32_t setLayoutCount, const RDescriptorSetLayout* const* pSetLayouts, uint32_t pushConstantRangeCount, const RSPushConstantRange* pPushConstantRanges)
	{
		uint32_t i;
		VkPipelineLayoutCreateInfo info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
		info.flags = 0;
		info.pNext = nullptr;
		info.setLayoutCount = setLayoutCount;
		if(setLayoutCount > 0) {
			TArray<VkDescriptorSetLayout> setLayoutsVk(setLayoutCount);
			for (i = 0; i < setLayoutCount; ++i) {
				setLayoutsVk[i] = ((RDescriptorSetLayoutVk*)pSetLayouts[i])->handle;
			}
			info.pSetLayouts = setLayoutsVk.Data();
		}
		info.pushConstantRangeCount = pushConstantRangeCount;
		if(pushConstantRangeCount > 0) {
			TArray<VkPushConstantRange> pushConstantRangesVk(pushConstantRangeCount);
			for(i=0; i< pushConstantRangeCount; ++i) {
				pushConstantRangesVk[i].offset = pPushConstantRanges[i].offset;
				pushConstantRangesVk[i].size = pPushConstantRanges[i].size;
				pushConstantRangesVk[i].stageFlags = pPushConstantRanges[i].stageFlags;
			}
			info.pPushConstantRanges = pushConstantRangesVk.Data();
		}

		VkPipelineLayout handle;
		if(VK_SUCCESS != vkCreatePipelineLayout(m_Device, &info, nullptr, &handle)) {
			return nullptr;
		}
		RPipelineLayoutVk* pipelineLayout = new RPipelineLayoutVk;
		pipelineLayout->handle = handle;
		return pipelineLayout;
	}

	void RHIVulkan::DestroyPipelineLayout(RPipelineLayout* pipelineLayout)
	{
		RPipelineLayoutVk* pipelineLayoutVk = (RPipelineLayoutVk*)pipelineLayout;
		vkDestroyPipelineLayout(m_Device, pipelineLayoutVk->handle, nullptr);
		delete pipelineLayoutVk;
	}

	RPipeline* RHIVulkan::CreateGraphicsPipeline(const RGraphicsPipelineCreateInfo& info, RPipelineLayout* layout, RRenderPass* renderPass, uint32_t subpass, RPipeline* basePipeline, int32_t basePipelineIndex)
	{
		uint32_t i; // iter
		// shader stages
		TArray<VkShaderModuleCreateInfo> shaderModuleInfos(info.shaderCount);
		TArray<VkShaderModule> shaderModules(info.shaderCount);
		TArray<VkPipelineShaderStageCreateInfo> shaderInfos(info.shaderCount);
		for (i = 0; i < info.shaderCount; ++i) {
			shaderModuleInfos[i] = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0 };
			shaderModuleInfos[i].codeSize = info.shaders[i].codeSize;
			shaderModuleInfos[i].pCode = info.shaders[i].pCode;
			vkCreateShaderModule(m_Device, &shaderModuleInfos[i], nullptr, &shaderModules[i]);

			shaderInfos[i] = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0 };
			shaderInfos[i].module = shaderModules[i];
			shaderInfos[i].pName = info.shaders[i].funcName;
			shaderInfos[i].stage = (VkShaderStageFlagBits)info.shaders[i].stage;
		}

		// vertex input
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO, nullptr, 0 };
		TArray<VkVertexInputBindingDescription> vertexInputBindings(info.bindingCount);
		for(i=0; i< info.bindingCount; ++i) {
			vertexInputBindings[i].binding = info.bindings[i].binding;
			vertexInputBindings[i].inputRate = (VkVertexInputRate)info.bindings[i].inputRate;
		}
		vertexInputInfo.vertexBindingDescriptionCount = info.bindingCount;
		vertexInputInfo.pVertexBindingDescriptions = vertexInputBindings.Data();
		TArray<VkVertexInputAttributeDescription> vertexInputAttrs(info.attributeCount);
		for(i=0; i< info.attributeCount; ++i) {
			vertexInputAttrs[i].location = info.attributes[i].location;
			vertexInputAttrs[i].binding = info.attributes[i].binding;
			vertexInputAttrs[i].format = (VkFormat)info.attributes[i].format;
			vertexInputAttrs[i].offset = info.attributes[i].offset;
		}
		vertexInputInfo.vertexAttributeDescriptionCount = info.attributeCount;
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttrs.Data();

		// input assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO, nullptr, 0 };
		inputAssemblyInfo.primitiveRestartEnable = info.primitiveRestartEnable;
		inputAssemblyInfo.topology = (VkPrimitiveTopology)info.topology;

		// tessellation
		VkPipelineTessellationStateCreateInfo tessellationInfo{ VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO, nullptr, 0 };
		tessellationInfo.patchControlPoints = info.patchControlPoints;

		// viewport
		VkPipelineViewportStateCreateInfo viewportInfo{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO, nullptr, 0 };
		auto& viewport = info.viewport;
		VkViewport viewportVk{ viewport.x, viewport.y, viewport.width, viewport.height, viewport.minDepth, viewport.maxDepth };
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &viewportVk;
		auto& scissor = info.Scissor;
		VkRect2D scissorVk{ {scissor.offset.x, scissor.offset.y}, {scissor.extent.width, scissor.extent.height} };
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &scissorVk;

		// rasterization
		VkPipelineRasterizationStateCreateInfo rasterizationInfo = TranslateVkPipelineRasterizationState(info);
		// multi sample
		VkPipelineMultisampleStateCreateInfo multisampleInfo = TranslatePipelineMultisample(info);
		// depth stencil
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo = TranslatePipelineDepthStencil(info);

		// color blend
		VkPipelineColorBlendStateCreateInfo colorBlendInfo{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO, nullptr, 0 };
		colorBlendInfo.logicOpEnable = info.logicOpEnable;
		colorBlendInfo.logicOp = (VkLogicOp)info.logicOp;
		colorBlendInfo.attachmentCount = info.attachmentCount;
		TArray<VkPipelineColorBlendAttachmentState> attachiments(info.attachmentCount);
		for(i=0; i<info.attachmentCount; ++i) {
			TranslateColorBlendAttachmentState(attachiments[i], info.pAttachments[i]);
		}
		colorBlendInfo.blendConstants[0] = info.blendConstants[0];
		colorBlendInfo.blendConstants[1] = info.blendConstants[1];
		colorBlendInfo.blendConstants[2] = info.blendConstants[2];
		colorBlendInfo.blendConstants[3] = info.blendConstants[3];

		// dynamic
		VkPipelineDynamicStateCreateInfo dynamicInfo{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO, nullptr, 0 };
		dynamicInfo.dynamicStateCount = info.dynamicStateCount;
		dynamicInfo.pDynamicStates = (VkDynamicState*)info.pDynamicStates;

		// pipeline
		VkGraphicsPipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO, nullptr, 0 };
		createInfo.stageCount = info.shaderCount;
		createInfo.pStages = shaderInfos.Data();
		createInfo.pVertexInputState = &vertexInputInfo;
		createInfo.pInputAssemblyState = &inputAssemblyInfo;
		createInfo.pTessellationState = &tessellationInfo;
		createInfo.pViewportState = &viewportInfo;
		createInfo.pRasterizationState = &rasterizationInfo;
		createInfo.pMultisampleState = &multisampleInfo;
		createInfo.pDepthStencilState = &depthStencilInfo;
		createInfo.pColorBlendState = &colorBlendInfo;
		createInfo.pDynamicState = &dynamicInfo;
		createInfo.layout = ((RPipelineLayoutVk*)layout)->handle;
		createInfo.renderPass = ((RRenderPassVk*)renderPass)->handle;
		createInfo.subpass = subpass;
		createInfo.basePipelineHandle = nullptr == basePipeline ? VK_NULL_HANDLE : ((RPipelineVk*)basePipeline)->handle;
		createInfo.basePipelineIndex = basePipelineIndex;

		VkPipeline handle;
		VkResult res = vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle);
		for(i=0; i< info.shaderCount; ++i) {
			vkDestroyShaderModule(m_Device, shaderModules[i], nullptr);
		}
		if(VK_SUCCESS != res) {
			return nullptr;
		}
		RPipelineVk* pipeline = new RPipelineVk;
		pipeline->handle = handle;
		pipeline->m_Type = PIPELINE_GRAPHICS;
		return pipeline;
	}

	RPipeline* RHIVulkan::CreateComputePipeline(const RPipelineShaderInfo& shader, RPipelineLayout* layout, RPipeline* basePipeline, uint32_t basePipelineIndex)
	{
		VkComputePipelineCreateInfo createInfo{ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO, nullptr, 0 };
		// shader stages
		VkShaderModuleCreateInfo shaderModuleInfo;
		VkShaderModule shaderModule;
		VkPipelineShaderStageCreateInfo& shaderInfo = createInfo.stage;
		shaderModuleInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO, nullptr, 0 };
		shaderModuleInfo.codeSize = shader.codeSize;
		shaderModuleInfo.pCode = shader.pCode;
		vkCreateShaderModule(m_Device, &shaderModuleInfo, nullptr, &shaderModule);

		shaderInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0 };
		shaderInfo.module = shaderModule;
		shaderInfo.pName = shader.funcName;
		shaderInfo.stage = (VkShaderStageFlagBits)shader.stage;

		createInfo.layout = ((RPipelineLayoutVk*)layout)->handle;
		createInfo.basePipelineHandle = nullptr == basePipeline ? VK_NULL_HANDLE : ((RPipelineVk*)basePipeline)->handle;
		createInfo.basePipelineIndex = basePipelineIndex;

		VkPipeline handle;
		VkResult res = vkCreateComputePipelines(m_Device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle);
		vkDestroyShaderModule(m_Device, shaderModule, nullptr);
		if(VK_SUCCESS != res) {
			return nullptr;
		}
		RPipelineVk* pipeline = new RPipelineVk;
		pipeline->handle = handle;
		pipeline->m_Type = PIPELINE_COMPUTE;
		return pipeline;
	}

	void RHIVulkan::DestroyPipeline(RPipeline* pipeline)
	{
		RPipelineVk* pipelineVk = (RPipelineVk*)pipeline;
		vkDestroyPipeline(m_Device, pipelineVk->handle, nullptr);
		delete pipelineVk;
	}

	RQueue* RHIVulkan::GetGraphicsQueue()
	{
		return reinterpret_cast<RQueue*>(&m_GraphicsQueue);
	}

	void RHIVulkan::ResizeSwapchain(uint32_t width, uint32_t height)
	{
		if (VK_SUCCESS != _vkWaitForFences(m_Device, m_IsFrameInFlightFences.size(), m_IsFrameInFlightFences.data(), VK_TRUE, UINT64_MAX))
		{
			ERROR("_vkWaitForFences failed");
			return;
		}
		m_SwapchainExtent.width = static_cast<uint32_t>(width);
		m_SwapchainExtent.height = static_cast<uint32_t>(height);
		ClearSwapchain();
		CreateSwapchain();
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
		TArray<VkSemaphore> vkWaitSmps(waitSemaphoreCount);
		for (i = 0; i < waitSemaphoreCount; ++i) {
			vkWaitSmps[i] = reinterpret_cast<RSemaphoreVk*>(waitSemaphores + i)->handle;
		}
		submitInfo.waitSemaphoreCount = waitSemaphoreCount;
		submitInfo.pWaitSemaphores = vkWaitSmps.Data();
		submitInfo.pWaitDstStageMask = (VkPipelineStageFlags*)waitStageFlags;

		// commands
		TArray<VkCommandBuffer> vkCmds(cmdCount);
		for (i = 0; i < cmdCount; ++i) {
			vkCmds[i] = reinterpret_cast<RCommandBufferVk*>(cmds + i)->handle;
		}
		submitInfo.commandBufferCount = cmdCount;
		submitInfo.pCommandBuffers = vkCmds.Data();

		// signal semaphores
		TArray<VkSemaphore> vkSignalSmps(signalSemaphoreCount);
		for(i=0; i< signalSemaphoreCount; ++i) {
			vkSignalSmps[i] = reinterpret_cast<RSemaphoreVk*>(signalSemaphores + i)->handle;
		}
		submitInfo.signalSemaphoreCount = signalSemaphoreCount;
		submitInfo.pSignalSemaphores = vkSignalSmps.Data();

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
		ASSERT(i < m_SwapchainImageViews.size(), "i < m_MaxFramesInFlight");
		return reinterpret_cast<RImageView*>(&m_SwapchainImageViews[i]);
	}
	uint32_t RHIVulkan::GetSwapchainMaxImageCount()
	{
		return (uint32_t)m_SwapchainImages.size();
	}
	RFramebuffer* RHIVulkan::CreateFrameBuffer(RRenderPass* pass, uint32_t imageViewCount, const RImageView* const* pImageViews, uint32_t width, uint32_t height, uint32_t layers)
	{
		VkFramebufferCreateInfo framebufferInfo{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
		framebufferInfo.renderPass = reinterpret_cast<RRenderPassVk*>(pass)->handle;
		TArray<VkImageView> vkImageViews(imageViewCount);
		for(uint32_t i=0; i< imageViewCount; ++i) {
			const RImageViewVk* imageViewVk = (const RImageViewVk*)(pImageViews[i]);
			vkImageViews[i] = imageViewVk->handle;
		}
		framebufferInfo.attachmentCount = imageViewCount;
		framebufferInfo.pAttachments = vkImageViews.Data();
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

	void RHIVulkan::DestroyFramebuffer(RFramebuffer* framebuffer)
	{
		RFramebufferVk* framebufferVk = (RFramebufferVk*)framebuffer;
		vkDestroyFramebuffer(m_Device, framebufferVk->handle, nullptr);
		delete framebufferVk;
	}
	RCommandBuffer* RHIVulkan::AllocateCommandBuffer(RCommandBufferLevel level)
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
	void RHIVulkan::CmdBeginRenderPass(RCommandBuffer* cmd, RRenderPass* pass, RFramebuffer* framebuffer, RSRect2D renderArea, uint32_t clearValueCount, const RSClear* clearValues)
	{
		VkRect2D vkRenderArea{ {renderArea.offset.x, renderArea.offset.y}, {renderArea.extent.width, renderArea.extent.height} };
		VkRenderPassBeginInfo passInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		passInfo.pNext = nullptr;
		passInfo.renderPass = reinterpret_cast<RRenderPassVk*>(pass)->handle;
		passInfo.framebuffer = reinterpret_cast<RFramebufferVk*>(framebuffer)->handle;
		passInfo.renderArea = vkRenderArea;
		passInfo.clearValueCount = clearValueCount;
		TArray<VkClearValue> clearValuesVk(clearValueCount);
		for(uint32_t i=0; i<clearValueCount; ++i) {
			if(clearValues[i].clearType == CLEAR_VALUE_COLOR) {
				clearValuesVk[i].color.float32[0] = clearValues[i].clearValue.color.r;
				clearValuesVk[i].color.float32[1] = clearValues[i].clearValue.color.g;
				clearValuesVk[i].color.float32[2] = clearValues[i].clearValue.color.b;
				clearValuesVk[i].color.float32[3] = clearValues[i].clearValue.color.a;
			}
			else {
				clearValuesVk[i].depthStencil.depth = clearValues[i].clearValue.depthStencil.depth;
				clearValuesVk[i].depthStencil.stencil = clearValues[i].clearValue.depthStencil.stencil;
			}
		}
		passInfo.pClearValues = clearValuesVk.Data();
		_vkCmdBeginRenderPass(((RCommandBufferVk*)cmd)->handle, &passInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	void RHIVulkan::CmdEndRenderPass(RCommandBuffer* cmd)
	{
		_vkCmdEndRenderPass(((RCommandBufferVk*)cmd)->handle);
	}
	void RHIVulkan::CmdTransitionImageLayout(RCommandBuffer* cmd, RImage* image, RImageLayout oldLayout, RImageLayout newLayout, uint32_t baseLevel, uint32_t levelCount, uint32_t baseLayer, uint32_t layerCount, RImageAspectFlags aspect)
	{
		RImageVk* imageVk = (RImageVk*)image;
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = (VkImageLayout)oldLayout;
		barrier.newLayout = (VkImageLayout)newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = imageVk->handle;
		barrier.subresourceRange.aspectMask = aspect;
		barrier.subresourceRange.baseMipLevel = baseLevel;
		barrier.subresourceRange.levelCount = levelCount;
		barrier.subresourceRange.baseArrayLayer = baseLayer;
		barrier.subresourceRange.layerCount = layerCount;
		VkPipelineStageFlags srcStage;
		VkPipelineStageFlags dstStage;
		GetPipelineBarrierStage(barrier.oldLayout, barrier.newLayout, barrier.srcAccessMask, barrier.dstAccessMask, srcStage, dstStage);
		vkCmdPipelineBarrier(((RCommandBufferVk*)cmd)->handle, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
		imageVk->m_Layout = newLayout;
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

	void RHIVulkan::CmdBindPipeline(RCommandBuffer* cmd, RPipeline* pipeline)
	{
		RPipelineVk* pipelineVk = (RPipelineVk*)pipeline;
		_vkCmdBindPipeline(((RCommandBufferVk*)cmd)->handle, (VkPipelineBindPoint)pipelineVk->GetType(), pipelineVk->handle);
	}

	void RHIVulkan::CmdBindDescriptorSet(RCommandBuffer* cmd, RPipelineType pipelineType, RPipelineLayout* layout, RDescriptorSet* descriptorSet, uint32_t firstSet)
	{
		VkCommandBuffer handle = ((RCommandBufferVk*)cmd)->handle;
		_vkCmdBindDescriptorSets(handle, (VkPipelineBindPoint)pipelineType, ((RPipelineLayoutVk*)layout)->handle,
			firstSet, 1, &((RDescriptorSetVk*)descriptorSet)->handle, 0, nullptr);
	}

	void RHIVulkan::CmdBindVertexBuffer(RCommandBuffer* cmd, RBuffer* buffer, uint32_t first, size_t offset)
	{
		VkCommandBuffer handle = ((RCommandBufferVk*)cmd)->handle;
		_vkCmdBindVertexBuffers(handle, first, 1, &((RBufferVk*)buffer)->handle, &offset);
	}

	void RHIVulkan::CmdBindIndexBuffer(RCommandBuffer* cmd, RBuffer* buffer, size_t offset)
	{
		VkCommandBuffer handle = ((RCommandBufferVk*)cmd)->handle;
		_vkCmdBindIndexBuffer(handle, ((RBufferVk*)buffer)->handle, offset, VK_INDEX_TYPE_UINT32);
	}

	void RHIVulkan::CmdDraw(RCommandBuffer* cmd, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance)
	{
		VkCommandBuffer handle = ((RCommandBufferVk*)cmd)->handle;
		_vkCmdDraw(handle, vertexCount, instanceCount, firstIndex, firstInstance);
	}

	void RHIVulkan::CmdDrawIndexed(RCommandBuffer* cmd, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
	{
		VkCommandBuffer handle = ((RCommandBufferVk*)cmd)->handle;
		_vkCmdDrawIndexed(handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void RHIVulkan::CmdDispatch(RCommandBuffer* cmd, uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
	{
		_vkCmdDispatch(((RCommandBufferVk*)cmd)->handle, groupCountX, groupCountY, groupCountZ);
	}

	void RHIVulkan::CmdClearAttachment(RCommandBuffer* cmd, RImageAspectFlags aspect, const float* color, const RSRect2D& rect)
	{
		VkClearAttachment clearAttachment;
		clearAttachment.aspectMask = aspect;
		clearAttachment.clearValue.color.float32[0] = color[0];
		clearAttachment.clearValue.color.float32[1] = color[1];
		clearAttachment.clearValue.color.float32[2] = color[2];
		clearAttachment.clearValue.color.float32[3] = color[3];
		VkClearRect clearRect;
		clearRect.rect.extent.width = rect.extent.width;
		clearRect.rect.extent.height = rect.extent.height;
		clearRect.rect.offset.x = rect.offset.x;
		clearRect.rect.offset.y = rect.offset.y;
		clearRect.baseArrayLayer = 0;
		clearRect.layerCount = 1;
		_vkCmdClearAttachments(((RCommandBufferVk*)cmd)->handle, 1, &clearAttachment, 1, &clearRect);
	}

	void RHIVulkan::CmdCopyBuffer(RCommandBuffer* cmd, RBuffer* srcBuffer, RBuffer* dstBuffer, size_t srcOffset, size_t dstOffset, size_t size)
	{
		VkBufferCopy copy{ srcOffset, dstOffset, size };
		vkCmdCopyBuffer(((RCommandBufferVk*)cmd)->handle,
			((RBufferVk*)srcBuffer)->handle, ((RBufferVk*)dstBuffer)->handle, 1, &copy);
	}

	void RHIVulkan::CmdBeginDebugLabel(RCommandBuffer* cmd, const char* msg, const float* color)
	{
		if(m_EnableDebugUtils) {
			VkDebugUtilsLabelEXT labelInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT, nullptr };
			labelInfo.pLabelName = msg;
			if(nullptr != color) {
				for(int i=0; i<4; ++i) {
					labelInfo.color[i] = color[i];
				}
			}
			_vkCmdBeginDebugUtilsLabelEXT(((RCommandBufferVk*)cmd)->handle, &labelInfo);
		}
	}

	void RHIVulkan::CmdEndDebugLabel(RCommandBuffer* cmd)
	{
		if(m_EnableDebugUtils) {
			_vkCmdEndDebugUtilsLabelEXT(((RCommandBufferVk*)cmd)->handle);
		}
	}

	void RHIVulkan::ImmediateCommit(const CommandBufferFunc& func)
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

	int RHIVulkan::PreparePresent(uint8_t frameIndex)
	{
		_vkWaitForFences(m_Device, 1, &m_IsFrameInFlightFences[frameIndex], VK_TRUE, UINT64_MAX);
		VkResult res = vkAcquireNextImageKHR(m_Device, m_Swapchain, UINT64_MAX, m_ImageAvaliableSemaphores[frameIndex], VK_NULL_HANDLE, &m_CurrentSwapchainImageIndex);
		if(VK_ERROR_OUT_OF_DATE_KHR == res){
			return -1;
		}
		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) {
			ERROR("failed to acquire swap chain image!");
		}


		VK_CHECK(_vkResetCommandPool(m_Device, m_CommandPools[frameIndex], 0), "Failed to reset command pool!");
		return static_cast<int>(m_CurrentSwapchainImageIndex);
	}
	int RHIVulkan::QueueSubmitPresent(RCommandBuffer* cmd, uint8_t frameIndex)
	{
		VK_CHECK(_vkResetFences(m_Device, 1, &m_IsFrameInFlightFences[frameIndex]));

		// submit command buffer
		//VkSemaphore semaphores[2] = { m_ImageAvaliableForTextureCopySemaphores[m_CurrentFrame], m_PresentationFinishSemaphores[m_CurrentFrame] };
		VkSemaphore semaphores[1] = { m_PresentationFinishSemaphores[frameIndex] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
		VkCommandBuffer vkCmd = reinterpret_cast<RCommandBufferVk*>(cmd)->handle;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_ImageAvaliableSemaphores[frameIndex];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vkCmd;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = semaphores;
		VK_CHECK(vkQueueSubmit(m_GraphicsQueue.handle, 1, &submitInfo, m_IsFrameInFlightFences[frameIndex]), "Failed to submit graphics queue!");

		// present
		VkPresentInfoKHR presentQueue = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
		presentQueue.waitSemaphoreCount = 1;
		presentQueue.pWaitSemaphores = &m_PresentationFinishSemaphores[frameIndex];
		presentQueue.swapchainCount = 1;
		presentQueue.pSwapchains = &m_Swapchain;
		presentQueue.pImageIndices = &m_CurrentSwapchainImageIndex;

		VkResult presentResult = vkQueuePresentKHR(m_PresentQueue.handle, &presentQueue);
		if (VK_ERROR_OUT_OF_DATE_KHR == presentResult || VK_SUBOPTIMAL_KHR == presentResult) {
			return -1;
		}
		return 0;
	}
	void RHIVulkan::FreeMemory(RMemory* memory)
	{
		vmaFreeMemory(m_Vma, reinterpret_cast<RMemoryVma*>(memory)->handle);
		delete memory;
	}

	RBuffer* RHIVulkan::CreateBuffer(size_t size, RBufferUsageFlags usage)
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

	RMemory* RHIVulkan::CreateBufferMemory(RBuffer* buffer, RMemoryPropertyFlags memoryProperty, size_t dataSize, void* pData)
	{
		VmaAllocationCreateInfo info;
		info.flags = 0;
		info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		info.requiredFlags = (VkMemoryPropertyFlags)memoryProperty;
		info.preferredFlags = (VkMemoryPropertyFlags)memoryProperty;
		info.memoryTypeBits = 0;
		info.pool = nullptr;
		info.pUserData = pData;
		info.priority = 1.0f;
		VmaAllocation handle; 
		if(VK_SUCCESS != vmaAllocateMemoryForBuffer(m_Vma, ((RBufferVk*)buffer)->handle, &info, &handle, nullptr)) {
			return nullptr;
		}
		RMemoryVma* memory = new RMemoryVma;
		memory->handle = handle;
		return memory;
	}

	void RHIVulkan::CreateBufferWithMemory(size_t size, RBufferUsageFlags usage, RMemoryPropertyFlags memoryFlags, RBuffer*& pBuffer, RMemory*& pMemory, size_t dataSize, void* pData)
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
		image->m_Layout = IMAGE_LAYOUT_UNDEFINED;
		return image;
	}

	RImage* RHIVulkan::CreateImage2D(RFormat format, uint32_t width, uint32_t height, uint32_t mipLevels, RSampleCountFlagBits samples, RImageTiling tiling, RImageUsageFlags usage)
	{
		return CreateImage(IMAGE_TYPE_2D, format, {width, height, 1}, mipLevels, 1, samples, tiling, usage);
	}

	RMemory* RHIVulkan::CreateImageMemory(RImage* image, RMemoryPropertyFlags memoryProperty, void* pData)
	{
		VkImage imageVk = reinterpret_cast<RImageVk*>(image)->handle;
		VmaAllocationCreateInfo info;
		info.flags = 0;
		info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		info.requiredFlags = (VkMemoryPropertyFlags)memoryProperty;
		info.preferredFlags = (VkMemoryPropertyFlags)memoryProperty;
		info.memoryTypeBits = 0;
		info.pool = nullptr;
		info.pUserData = pData;
		info.priority = 1.0f;
		VmaAllocation handle;
		if (VK_SUCCESS != vmaAllocateMemoryForImage(m_Vma, imageVk, &info, &handle, nullptr) ||
			VK_SUCCESS != vmaBindImageMemory(m_Vma, handle, imageVk)) {
			return nullptr;
		}
		RMemoryVma* memory = new RMemoryVma;
		memory->handle = handle;
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
		RDescriptorInfo desInfo;

		RImageViewVk* imageView = new RImageViewVk;
		imageView->handle = handle;
		imageView->m_Layout = image->GetLayout();
		return imageView;
	}

	void RHIVulkan::DestroyImageView(RImageView* imageView)
	{
		RImageViewVk* imageViewVk = (RImageViewVk*)imageView;
		vkDestroyImageView(m_Device, imageViewVk->handle, nullptr);
		delete imageViewVk;
	}

	RSampler* RHIVulkan::CreateSampler(const RSSamplerInfo& samplerInfo)
	{
		VkSamplerCreateInfo info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		info.pNext = nullptr;
		info.flags = 0;
		info.magFilter = (VkFilter)samplerInfo.magFilter;
		info.minFilter = (VkFilter)samplerInfo.minFilter;
		info.addressModeU = (VkSamplerAddressMode)samplerInfo.addressModeU;
		info.addressModeV = (VkSamplerAddressMode)samplerInfo.addressModeV;
		info.addressModeW = (VkSamplerAddressMode)samplerInfo.addressModeW;
		info.mipLodBias = samplerInfo.minLodBias;
		info.anisotropyEnable = samplerInfo.anisotropyEnable;
		info.maxAnisotropy = samplerInfo.maxAnisotropy;
		info.compareEnable = samplerInfo.compareEnable;
		info.compareOp = (VkCompareOp)samplerInfo.compreOp;
		info.minLod = samplerInfo.minLod;
		info.maxLod = samplerInfo.maxLod;
		info.borderColor = (VkBorderColor)samplerInfo.borderColor;
		info.unnormalizedCoordinates = samplerInfo.unnormalizedCoordinates;

		VkSampler handle;
		if(VK_SUCCESS != vkCreateSampler(m_Device, &info, nullptr, &handle)) {
			return nullptr;
		}
		RETURN_RHI_PTR(RSampler, handle);
	}

} // namespace RHI
