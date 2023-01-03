#pragma once
#include "UIBase.h"
#include "RHI/RHIInstance.h"
#include "UIRenderer.h"


namespace Engine {
	class WindowSystemBase;

	class RenderSystem {
	private:
		//RenderPasses
		RHI::RRenderPass m_MainPass;
		UIRenderer m_UIRenderer;
	public:
		RenderSystem() = default;
		RenderSystem(WindowSystemBase* window);
		~RenderSystem();
		void Tick();
		void InitUIPass(UIBase* ui);
	};
}