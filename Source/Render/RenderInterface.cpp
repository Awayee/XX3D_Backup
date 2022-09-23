#include <iostream>
#include "RenderInterface.h"
#include "Vulkan\VulkanCore.h"
namespace RI {
	std::unique_ptr<RenderCore> s_RenderCore;

	void Initialize(GLFWwindow* w)
	{
		s_RenderCore = std::make_unique<VulkanCore>(w);
	}
	void Release()
	{
		s_RenderCore.reset();
	}
}