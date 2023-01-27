#pragma once
#include "Engine/Engine.h"
#include "EditorUI.h"
#include "Core/Memory/SmartPointer.h"

#include "Engine/Mesh/RenderMesh.h"

namespace Editor {
	class XXEditor {
	private:
		Engine::XXEngine* m_Engine;
		TUniquePtr<EditorUI> m_EditorUI;
		// TODO TEST
		TUniquePtr<Engine::RenderMesh> m_Ms;

	private:
		void LoadMesh();
		
	public:
		XXEditor(Engine::XXEngine* engine);
		~XXEditor();
		void EditorRun();
	};
}