#pragma once
#include "Core/Public/TPtr.h"
#include "Render/Public/RenderReses.h"
#include "Asset/Public/LevelAsset.h"


namespace Engine {
	class Camera;
	class Material;
	class DirectionalLight;
	class RenderScene;

	class RenderObject {
		friend RenderScene;
		static constexpr uint32 INVALID_INDEX = UINT32_MAX;
	protected:
		RenderScene* m_Scene;
		uint32 m_Index;
	public:
		virtual void DrawCall(Engine::RCommandBuffer* cmd, Engine::RPipelineLayout* layout) = 0;
		virtual void Update() = 0;
		RenderObject(RenderScene* scene);
		virtual ~RenderObject();
	};

	class RenderScene {
	private:
		friend class Renderer;
		TVector<RenderObject*> m_RenderObjects; // all render objects
		TUniquePtr<DirectionalLight> m_DirectionalLight;
		TUniquePtr<Camera> m_Camera;
		Engine::RDescriptorSet* m_SceneDescs;
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
		const TVector<RenderObject*> GetRenderObjects() const { return m_RenderObjects; }
		void AddRenderObject(RenderObject* obj);
		void RemoveRenderObject(RenderObject* obj);
		void Update();
		void RenderGBuffer(Engine::RCommandBuffer* cmd, Engine::RPipelineLayout* layout);
	};
}
