#pragma once
#include "UIBase.h"
#include "RHI/RHI.h"
#include "Core/Container/Container.h"
#include "Core/Memory/SmartPointer.h"
#include "RenderScene.h"
#include "Engine/Texture/Texture.h"

namespace Engine {
	class WindowSystemBase;

	enum ERenderPassType {
		PASS_MAIN,   // lighting
		//PASS_BASE, // normal
		PASS_COUNT,
	};

	class RenderSystem {
	private:

		enum ERenderImageType {
			ATTACHMENT_DEPTH,
			ATTACHMENT_NORMAL,
			ATTACHMENT_ALBEDO,
			ATTACHMENT_COUNT
		};
		//RenderPasses
		TVector<RHI::RRenderPass*> m_Passes;
		TVector<RHI::RCommandBuffer*> m_CommandBuffers;
		TVector<RHI::RFramebuffer*> m_SwapchianFramebuffers;

		TVector<TUniquePtr<Texture2D>> m_Images;

		// Render pipelines
		RHI::RPipeline* m_GBufferPipeline;
		RHI::RPipeline* m_ShadowPipeline;

		uint8_t m_CurrentFrameIndex{0};
		bool m_WindowAvailable{ true };
		UIBase* m_UIContent;

		// Render scenes
		TVector<TUniquePtr<RenderScene>> m_Scenes;
		RenderScene* m_MainScene;

	public:
		RenderSystem() = default;
		RenderSystem(WindowSystemBase* window);
		~RenderSystem();
		void Tick();
		void InitUIPass(UIBase* ui);

	private:
		void CreateImages();
		void CreateRenderPasses();
		void CreateFramebuffers();
		void CreateCommandBuffers();
		void OnWindowSizeChanged(uint32 w, uint32 h);
		void RenderSceneForward(RHI::RCommandBuffer* cmd);
		void RenderSceneDeferred(RHI::RCommandBuffer* cmd);
	};
}
