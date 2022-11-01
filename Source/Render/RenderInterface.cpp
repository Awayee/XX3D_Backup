#include <iostream>
#include "RenderInterface.h"
#include "Vulkan\VulkanCore.h"
namespace RHI {
	std::unique_ptr<RenderCore> s_RenderCore;

	void Initialize()
	{
		s_RenderCore = std::make_unique<RenderCoreVulkan>();
	}
	void Release()
	{
		s_RenderCore.reset();
	}
}