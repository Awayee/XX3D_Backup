#pragma once
#include "UIBase.h"


namespace RHI {
	class RRenderPass;
}

namespace Engine {
	class UIRenderer {
	private:
		UIBase* m_UIContent;
		RHI::RRenderPass* m_RenderPass;
	public:
		UIRenderer() = default;
		void Initialize(UIBase* ui, RHI::RRenderPass* pass = nullptr);
		void Release();
		void Tick();
	};
}