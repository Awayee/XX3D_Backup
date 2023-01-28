#pragma once
#include "../Render/RenderResources.h"
#include "Core/Memory/SmartPointer.h"


namespace Engine {
	class Camera;
	class DirectionalLight;
	class RenderScene;

	struct ObjectHandle {
		RenderScene* Scene;
		uint32 Index;
	};

	class RenderObject {
		friend RenderScene;
	private:
		ObjectHandle m_Handle;
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
		void CreateDescriptorSets();
	public:
		static TUniquePtr<RenderScene> s_Default;
		static RenderScene* GetDefaultScene(); // TODO TEST
		static void Clear() { if (s_Default) s_Default.reset(); }

		RenderScene();
		~RenderScene();
		ObjectHandle AddRenderObject(RenderObject* obj);
		void RemoveRenderObject(const ObjectHandle& handle);
		void RenderGBuffer(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout);
		void RenderDeferredLight(RHI::RCommandBuffer* cmd);
	};
}
