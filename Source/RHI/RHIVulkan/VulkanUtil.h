#pragma once
#include "Core/macro.h"
#include<vulkan/vulkan.h>
#include "Core/Container/Container.h"
#include "RHI/RHIClasses.h"

namespace RHI {

#define VK_CHECK(x, s)\
	if(VK_SUCCESS != x) throw s

	const uint32_t VK_API_VER = VK_API_VERSION_1_2;

#define VK_FREE(ptr) delete ptr; ptr = nullptr

	// debug callback
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* data)
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

	inline void SetDebugInfo(VkDebugUtilsMessengerCreateInfoEXT& debugInfo) {
		debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugInfo.pfnUserCallback = DebugCallback;
	}


	inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	inline void DestroyDebugUtilsMessengerEXT(VkInstance instance,
		VkDebugUtilsMessengerEXT     debugMessenger,
		const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	inline VkPipelineRasterizationStateCreateInfo TranslateVkPipelineRasterizationState(const RGraphicsPipelineCreateInfo& rhiInfo) {
		VkPipelineRasterizationStateCreateInfo rasterizatonInfo{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, nullptr, 0 };
		rasterizatonInfo.depthBiasClamp = rhiInfo.depthBiasClamp;
		rasterizatonInfo.rasterizerDiscardEnable = rhiInfo.rasterizerDiscardEnable;
		rasterizatonInfo.polygonMode = (VkPolygonMode)rhiInfo.polygonMode;
		rasterizatonInfo.cullMode = rhiInfo.cullMode;
		rasterizatonInfo.frontFace = rhiInfo.clockwise ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizatonInfo.depthBiasEnable = rhiInfo.depthBiasEnable;
		rasterizatonInfo.depthBiasSlopeFactor = rhiInfo.depthBiasConstantFactor;
		rasterizatonInfo.depthBiasClamp = rhiInfo.depthBiasClamp;
		rasterizatonInfo.depthBiasSlopeFactor = rhiInfo.depthBiasSlopeFactor;
		rasterizatonInfo.lineWidth = rhiInfo.lineWidth;
		return rasterizatonInfo;
	}

	inline void TranslateVkStencilOp(VkStencilOpState& vkState, const RStencilOpState& rhiState) {
		vkState.failOp = (VkStencilOp)rhiState.failOp;
		vkState.passOp = (VkStencilOp)rhiState.passOp;
		vkState.depthFailOp = (VkStencilOp)rhiState.depthFailOp;
		vkState.compareOp = (VkCompareOp)rhiState.compareOp;
		vkState.compareMask = rhiState.compareMask;
		vkState.writeMask = rhiState.writeMask;
		vkState.reference = rhiState.reference;
	}

	inline void TranslateColorBlendAttachmentState(VkPipelineColorBlendAttachmentState& vkState, const RColorBlendAttachmentState& rhiState) {
		vkState.blendEnable = rhiState.blendEnable;
		vkState.srcColorBlendFactor = (VkBlendFactor)rhiState.srcColorBlendFactor;
		vkState.dstColorBlendFactor = (VkBlendFactor)rhiState.dstColorBlendFactor;
		vkState.colorBlendOp = (VkBlendOp)rhiState.colorBlendOp;
		vkState.srcAlphaBlendFactor = (VkBlendFactor)rhiState.srcAlphaBlendFactor;
		vkState.dstAlphaBlendFactor = (VkBlendFactor)rhiState.dstAlphaBlendFactor;
		vkState.alphaBlendOp = (VkBlendOp)rhiState.alphaBlendOp;
		vkState.colorWriteMask = (VkColorComponentFlags)rhiState.colorWriteMask;
	}

	inline VkPipelineMultisampleStateCreateInfo TranslatePipelineMultisample(const RGraphicsPipelineCreateInfo& info) {
		VkPipelineMultisampleStateCreateInfo multisampleInfo{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO, nullptr, 0 };
		multisampleInfo.rasterizationSamples = (VkSampleCountFlagBits)info.rasterizationSamples;
		multisampleInfo.sampleShadingEnable = info.sampleShadingEnable;
		multisampleInfo.minSampleShading = info.minSampleShading;
		multisampleInfo.pSampleMask = info.pSampleMask;
		multisampleInfo.alphaToCoverageEnable = info.alphaToCoverageEnable;
		multisampleInfo.alphaToOneEnable = info.alphaToOneEnable;
		return multisampleInfo;
	}

	inline VkPipelineDepthStencilStateCreateInfo TranslatePipelineDepthStencil(const RGraphicsPipelineCreateInfo& info) {
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO, nullptr, 0 };
		depthStencilInfo.depthTestEnable = info.depthTestEnable;
		depthStencilInfo.depthWriteEnable = info.depthWriteEnable;
		depthStencilInfo.depthCompareOp = (VkCompareOp)info.depthCompareOp;
		depthStencilInfo.depthBoundsTestEnable = info.depthBoundsTestEnable;
		depthStencilInfo.stencilTestEnable = info.stencilTestEnable;
		TranslateVkStencilOp(depthStencilInfo.front, info.front);
		TranslateVkStencilOp(depthStencilInfo.back, info.back);
		depthStencilInfo.minDepthBounds = info.minDepthBounds;
		depthStencilInfo.maxDepthBounds = info.maxDepthBounds;
		return depthStencilInfo;
	}

	void FindQueueFamilyIndex(const VkPhysicalDevice& device, const VkSurfaceKHR& surface, int* pGraphicsIndex, int* pPresentIndex, int* pComputeIndex);

	bool CheckExtensionSupported(VkPhysicalDevice device, const TVector<const char*>& extensions);

	VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);

	bool CheckValidationLayerSupport(const TVector<const char*>& validationLayers);

	struct SPhyicalDeviceInfo {
		int score{ -1 };
		int graphicsIndex{ -1 };
		int presentIndex{ -1 };
		int computeIndex{ -1 };
		uint32_t imageCount{ 0 };
		VkSurfaceFormatKHR swapchainFormat{ VK_FORMAT_UNDEFINED };
		VkPresentModeKHR swapchainPresentMode{ VK_PRESENT_MODE_MAX_ENUM_KHR };
		VkSurfaceTransformFlagBitsKHR swapchainTransform{ VK_SURFACE_TRANSFORM_FLAG_BITS_MAX_ENUM_KHR };
		VkExtent2D swapchainExtent{ 0, 0 };
	};

	SPhyicalDeviceInfo GetPhysicalDeviceInfo(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, const TVector<const char*>& extensions);


	void GetPipelineBarrierStage(VkImageLayout oldLayout, VkImageLayout newLayout,
		VkAccessFlags& srcAccessMask, VkAccessFlags& dstAccessMask, VkPipelineStageFlags& srcStage, VkPipelineStageFlags& dstStage);

	void GenerateMipMap(VkCommandBuffer cmd, VkImage image, uint32_t levelCount, uint32_t width, uint32_t height,
		VkImageAspectFlags aspect, uint32_t baseLayer, uint32_t layerCount);

}