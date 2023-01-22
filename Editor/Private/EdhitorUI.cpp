#include "../Public/EditorUI.h"
#include "Core/macro.h"
#include <stb_image.h>
#include "Resource/Config/Config.h"
#include "RHI/RHI.h"

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

		ImGui::StyleColorsDark();
	}

	EditorUI::~EditorUI() {
		//ImGui::DestroyContext(m_Context); // todo solve
	}
	void EditorUI::Tick(){
		ShowEditorUI();
	}

	void EditorUI::ShowEditorUI()
	{
		// simply test
		//ImGui::Begin("Hello World!");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		//ImGui::Text("Hello World!");
		//ImGui::End();
		
		static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		
	}
}