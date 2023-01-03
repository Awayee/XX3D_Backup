#pragma once
#include "Engine/Engine.h"
#include "EditorUI.h"
#include <memory>

namespace Editor {
	class XXEditor {
	private:
		Engine::XXEngine* m_Engine;
		std::unique_ptr<Editor::EditorUI> m_EditorUI;
	public:
		XXEditor(Engine::XXEngine* engine);
		~XXEditor();
		void EditorRun();
	};
}