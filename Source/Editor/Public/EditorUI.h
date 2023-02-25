#pragma once
#include "Render/Common/ImGuiImpl.h"
#include "Render/Common/UIBase.h"
#include "Render/Window/WindowSystem.h"
#include "Engine.h"

namespace Editor {
	class EditorUI: public Engine::UIBase {
	private:
		Engine::XXEngine* m_Engine;
		ImGuiContext* m_Context{ nullptr };
		void ShowEditorUI();
		void ShowFPS();
	public:
		EditorUI(Engine::XXEngine* engine);
		~EditorUI();
		void Tick();
	};
}
