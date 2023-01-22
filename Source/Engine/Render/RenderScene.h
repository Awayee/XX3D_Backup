#pragma once
#include "RenderResources.h"


namespace Engine {
	class RenderScene {
	private:
		friend class RenderSystem;

		TVector<Primitive*> m_Primitives;
		int m_ID; // internal unique identity;
	public:
		RenderScene() = default;
		RenderScene(const RenderScene&) = default;
		RenderScene(RenderScene&&) = default;
		~RenderScene();
		void RenderGBuffer(RHI::RCommandBuffer* cmd);
		void RenderLight(RHI::RCommandBuffer* cmd);
	};
}