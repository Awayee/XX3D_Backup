#include "../Public/EditorUI.h"
#include "Core/macro.h"
#include "Resource/Config/Config.h"
#include "Resource/Assets/Assets.h"

namespace Editor {
	EditorUI::EditorUI(Engine::XXEngine* engine)
	{
		auto window = engine->Window();

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
		auto fontAsset = AssetMgr::LoadAsset<AFontAsset>(GetConfig()->GetDefaultFontPath());
		io.Fonts->AddFontFromFileTTF(fontAsset.Path.c_str(), contentScale * 16, nullptr, nullptr);
		ASSERT(io.Fonts->Build(), "Failed to build fonts");
		io.IniFilename = nullptr; // Do not save settings

		//ImGuiStyle& style = ImGui::GetStyle();
		//style.WindowPadding = ImVec2(1.0, 0);
		//style.FramePadding = ImVec2(14.0, 2.0f);
		//style.ChildBorderSize = 0.0f;
		//style.FrameRounding = 5.0f;
		//style.FrameBorderSize = 1.5f;

		ImGui::StyleColorsDark();
		m_Engine = engine;
	}

	EditorUI::~EditorUI() {
		//ImGui::DestroyContext(m_Context); // todo solve
	}
	void EditorUI::Tick(){
		ShowEditorUI();
		ShowFPS();
	}

	void EditorUI::ShowEditorUI()
	{
		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(main_viewport->WorkPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2((float)GetConfig()->GetWindowSize().w, (float)GetConfig()->GetWindowSize().h), ImGuiCond_Always);
		ImGuiWindowFlags   window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground |
			ImGuiConfigFlags_NoMouseCursorChange | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGui::Begin("Editor menu", nullptr, window_flags);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Menu"))
            {
                if (ImGui::MenuItem("Reload Current Level"))
                {
                }
                if (ImGui::MenuItem("Save Current Level"))
                {
                }
                if (ImGui::BeginMenu("Debug"))
                {
                    if (ImGui::BeginMenu("Animation"))
                    {
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Camera"))
                    {
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Game Object"))
                    {
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Exit"))
                {
					m_Engine->Window()->Close();//close the window
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Window"))
            {
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
		ImGui::End();
	}

	void EditorUI::ShowFPS() {
		ImGui::Begin("FPS");
		ImGui::Text("%.1f", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}
