#pragma once
#include "UIBase.h"
#include "RHI/RHI.h"
#include "Core/Container/Container.h"
#include "Core/Memory/SmartPointer.h"
#include "RenderScene.h"
#include "RenderCommon.h"

namespace Engine {
	class WindowSystemBase;

	enum EPassType {
		PASS_PRESENT,   // main render
		//PASS_BASE, // normal
		PASS_COUNT,
	};

	class RenderSystem {
	private:
		//RenderPasses
		TUniquePtr<PresentPass> m_PresentPass;

		TVector<RHI::RCommandBuffer*> m_CommandBuffers;

		// Render pipelines
		TUniquePtr<GBufferPipeline> m_GBufferPipeline;

		uint8_t m_CurrentFrameIndex{0};
		bool m_WindowAvailable{ true };
		UIBase* m_UIContent;

	public:
		RenderSystem() = default;
		RenderSystem(WindowSystemBase* window);
		~RenderSystem();
		void Tick();
		void InitUIPass(UIBase* ui);

	private:
		void CreateCommandBuffers();
		void OnWindowSizeChanged(uint32 w, uint32 h);
	};
}
