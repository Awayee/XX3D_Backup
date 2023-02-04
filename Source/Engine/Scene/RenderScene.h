#pragma once
#include "../Render/RenderResources.h"
#include "Core/Memory/SmartPointer.h"


namespace Engine {
	class Camera;
	class Material;
	class DirectionalLight;
	class RenderScene;

	class RenderObject {
		friend RenderScene;
	private:
		RenderScene* m_Scene;
		uint32 m_Index;
	public:
		RenderObject(RenderScene* scene);
		virtual ~RenderObject();
		virtual void DrawCall(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout) = 0;
	};

	struct SceneRenderData {
		const Material* Material;
		const BufferCommon* Uniform;
		TVector<Primitive> primitives;
	};

	class RenderScene {
	private:
		friend class RenderSystem;
		TVector<RenderObject*> m_RenderObjects; // all render objects
		TUniquePtr<DirectionalLight> m_DirectionalLight;
		TUniquePtr<Camera> m_Camera;
		RHI::RDescriptorSet* m_SceneDescs;
		BufferCommon m_LightUniform;
		BufferCommon m_CameraUniform;

	private:
		void UpdateUniform();
		void CreateResources();
		void CreateDescriptorSets();
	public:
		static TUniquePtr<RenderScene> s_Default;
		static RenderScene* GetDefaultScene(); // TODO TEST
		static void Clear() { if (s_Default) s_Default.reset(); }
		Camera* GetMainCamera() { return m_Camera.get(); }

		RenderScene();
		~RenderScene();
		void AddRenderObject(RenderObject* obj);
		void RemoveRenderObject(RenderObject* obj);
		void Update();
		void RenderGBuffer(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout);
	};
}
