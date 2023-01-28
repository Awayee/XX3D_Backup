#pragma once
#include "Engine/Engine.h"
#include "EditorUI.h"
#include "Core/Memory/SmartPointer.h"

#include "Engine/Mesh/RenderMesh.h"

namespace Editor {
	class XXEditor {
	private:
		// TODO TEST
		TUniquePtr<Engine::RenderMesh> m_Ms;
		Engine::XXEngine* m_Engine;
		TUniquePtr<EditorUI> m_EditorUI;

	private:
		void LoadMesh();
		
	public:
		XXEditor(Engine::XXEngine* engine);
		~XXEditor();
		void EditorRun();
	};
}