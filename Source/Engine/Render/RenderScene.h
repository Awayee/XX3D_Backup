#pragma once
#include "RenderResources.h"
#include "Core/Memory/SmartPointer.h"
#include "../Camera/Camera.h"
#include "../Light/DirectionalLight.h"


namespace Engine {
	class RenderScene;

	class RenderObject {
		friend RenderScene;
	private:
		RenderScene* Scene;
		int32 Index;
	public:
		RenderObject(RenderScene* scene);
		virtual ~RenderObject();
		virtual void DrawCall(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout) = 0;
	};

	class RenderScene {
	private:
		friend class RenderSystem;
		TVector<RenderObject*> m_RenderObjects; // all render objects
		TUniquePtr<DirectionalLight> m_DirectionalLight;
		TUniquePtr<Camera> m_Camera;
		RHI::RDescriptorSet* m_SceneDescs;
		BufferCommon m_SceneUniform;
		BufferCommon m_CameraUniform;

	private:
		void UpdateUniform();
		void CreateResources();
	public:
		static TUniquePtr<RenderScene> s_Default;
		static RenderScene* GetDefaultScene(); // TODO TEST
		RenderScene();
		~RenderScene();
		void AddRenderObject(RenderObject* obj);
		void RemoveRenderObject(RenderObject* obj);
		void RenderGBuffer(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout);
		void RenderLight(RHI::RCommandBuffer* cmd);
	};
}
