#pragma once
#include "Render/Scene/RenderScene.h"
#include "FPSRecorder/FPSRecorder.h"
namespace Editor {

	class EditorContext {
	private:
		friend class XXEditor;
		Engine::RenderScene* m_Scene{nullptr};
		TUniquePtr<Engine::FPSRecorder> m_FPSRecorder;
	public:
		Engine::RenderScene* CurrentScene() { return m_Scene; }
		Engine::FPSRecorder* FPSRec() { return m_FPSRecorder.get(); }
	};

	EditorContext* Context();
}
