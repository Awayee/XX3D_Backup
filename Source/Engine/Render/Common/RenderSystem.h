#pragma once
#include "UIBase.h"
#include "RHI/RHI.h"
#include "Core/Memory/SmartPointer.h"
#include "RenderResources.h"
#include "Core/BaseType/Structs.h"

namespace Engine {
	class WindowSystemBase;

	enum EPassType {
		PASS_PRESENT,   // main render
		//PASS_BASE, // normal
		PASS_COUNT,
	};

	class RenderData {
		TVector<Primitive> Primitives;
	};

	class RenderSystem {
	private:
		bool m_Enable{ false };
		//Render Passes
		TUniquePtr<DeferredLightingPass> m_PresentPass;
		// command buffers
		TVector<RHI::RCommandBuffer*> m_CommandBuffers;
		// Render pipelines
		TUniquePtr<GBufferPipeline> m_GBufferPipeline;
		TUniquePtr<DeferredLightingPipeline> m_DeferredLightingPipeline;
		RHI::RDescriptorSet* m_DeferredLightingDescs;

		uint8_t m_CurrentFrameIndex{0};
		bool m_WindowAvailable{ true };
		URect2D m_RenderArea;
		UIBase* m_UIContent;

	public:
		RenderSystem() = default;
		RenderSystem(WindowSystemBase* window);
		~RenderSystem();
		void SetEnable(bool enable);
		void Tick();
		void InitUIPass(UIBase* ui);
		void SetRenderArea(URect2D&& area);

	private:
		void CreateRenderResources();
		void CreatePipelines();
		void RenderDeferredLighting(RHI::RCommandBuffer* cmd);
		void OnWindowSizeChanged(uint32 w, uint32 h);
	};
}
