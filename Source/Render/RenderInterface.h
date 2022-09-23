#pragma once
#include <memory>
#include <GLFW\glfw3.h>
#include "RenderCore.h"

namespace RI {

	extern std::unique_ptr<RenderCore> s_RenderCore;

	void Initialize(GLFWwindow* w);

	void Release();
}
