#include "../Public/EditorUI.h"
#include "Core/macro.h"
#include "Resource/Config/Config.h"
#include "RHI/RHI.h"

#include "Engine/Scene/RenderScene.h"
#include "Engine/Camera/Camera.h"

namespace Editor {
	EditorUI::EditorUI(Engine::WindowSystemBase* window)
	{
		m_Window = window;

		// imgui
		IMGUI_CHECKVERSION();
		m_Context = ImGui::CreateContext();

		float scaleX, scaleY;
		window->GetWindowContentScale(&scaleX, &scaleY);
		float contentScale = fmaxf(1.0f, fmaxf(scaleX, scaleY));

		// load font for imgui
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_None;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		io.Fonts->AddFontFromFileTTF(GetConfig()->GetDefaultFontPath().generic_string().c_str(), contentScale * 16, nullptr, nullptr);
		ASSERT(io.Fonts->Build(), "Failed to build fonts");
		io.IniFilename = nullptr; // Do not save settings

		//ImGuiStyle& style = ImGui::GetStyle();
		//style.WindowPadding = ImVec2(1.0, 0);
		//style.FramePadding = ImVec2(14.0, 2.0f);
		//style.ChildBorderSize = 0.0f;
		//style.FrameRounding = 5.0f;
		//style.FrameBorderSize = 1.5f;

		ImGui::StyleColorsLight();
	}

	EditorUI::~EditorUI() {
		//ImGui::DestroyContext(m_Context); // todo solve
	}
	void EditorUI::Tick(){
		ShowEditorUI();
	}

	void EditorUI::ShowEditorUI()
	{
		ImGui::Begin("FPS");
		ImGui::Text("%.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}