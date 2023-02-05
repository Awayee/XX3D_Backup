#pragma once
#include "imgui.h"
#include "Render/Common/UIBase.h"
#include "Render/Window/WindowSystem.h"

namespace Editor {
	class EditorUI: public Engine::UIBase {
	private:
		Engine::WindowSystemBase* m_Window;
		ImGuiContext* m_Context{ nullptr };
		void ShowEditorUI();
	public:
		EditorUI(Engine::WindowSystemBase* window);
		~EditorUI();
		void Tick();
	};
}
