#pragma once
#include <memory>
#include <GLFW\glfw3.h>

namespace RHI {


	void Initialize();

	void Release();

	class RenderCore {

	};
	extern std::unique_ptr<RenderCore> s_RenderCore;
}
