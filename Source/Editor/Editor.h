#pragma once
#include "EditorUI/EditorUI.h"
#include "Engine.h"
#include "Core/Memory/SmartPointer.h"
#include "Render/Mesh/RenderMesh.h"

namespace Editor {
	class XXEditor {
	private:
		// TODO TEST
		TUniquePtr<Engine::RenderMesh> m_Ms;
		Engine::RenderScene* m_MainScene;
		Engine::Camera* m_MainCamera;

		Engine::XXEngine* m_Engine;
		TUniquePtr<UIMgr> m_EditorUI;

	private:
		void InitResources();
	public:
		XXEditor(Engine::XXEngine* engine);
		~XXEditor();
		void EditorRun();
	};
}
