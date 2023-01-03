#pragma once
#include "Engine/Engine.h"
#include "Engine/Render/UIBase.h"
#include <imgui.h>

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