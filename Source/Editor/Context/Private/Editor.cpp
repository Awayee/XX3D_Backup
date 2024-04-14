﻿#include "Context/Public/Editor.h"
#include "Asset/Public/AssetLoader.h"
#include "Objects/Public/EngineContext.h"
#include "Resource/Public/Config.h"

#include "EditorUI/Public/UIController.h"
#include "EditorUI/Public/EditorUIMgr.h"
#include "Functions/Public/AssetManager.h"
#include "Functions/Public/EditorLevelMgr.h"
#include "Functions/Public/EditorTimer.h"
#include "Window/Public/Wnd.h"
#include "Window/Public/ImGuiImpl.h"
#include "Objects/Public/Renderer.h"

namespace Editor {
    void SetImGuiStyle()
    {
		ImGuiStyle& style = ImGui::GetStyle();
	    ImVec4* colors = style.Colors;
		//float h[3] = { 0.000f, 0.400f, 0.700f };// highlight color
		float h[3] = { 0.600f, 0.600f, 0.600f };// highlight color

	    colors[ImGuiCol_Text]                   = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	    colors[ImGuiCol_TextDisabled]           = ImVec4(0.500f, 0.500f, 0.500f, 1.000f);
	    colors[ImGuiCol_WindowBg]               = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	    colors[ImGuiCol_ChildBg]                = ImVec4(0.280f, 0.280f, 0.280f, 0.000f);
	    colors[ImGuiCol_PopupBg]                = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	    colors[ImGuiCol_Border]                 = ImVec4(0.266f, 0.266f, 0.266f, 1.000f);
	    colors[ImGuiCol_BorderShadow]           = ImVec4(0.000f, 0.000f, 0.000f, 0.000f);
	    colors[ImGuiCol_FrameBg]                = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	    colors[ImGuiCol_TitleBg]                = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_CheckMark]              = ImVec4(1.000f, 1.000f, 1.000f, 1.000f);
	    colors[ImGuiCol_SliderGrab]             = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	    colors[ImGuiCol_SliderGrabActive]       = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_Button]                 = ImVec4(1.000f, 1.000f, 1.000f, 0.000f);
	    colors[ImGuiCol_ButtonHovered]          = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	    colors[ImGuiCol_ButtonActive]           = ImVec4(1.000f, 1.000f, 1.000f, 0.391f);
	    colors[ImGuiCol_Header]                 = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	    colors[ImGuiCol_HeaderActive]           = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	    colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
	    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	    colors[ImGuiCol_SeparatorActive]        = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_ResizeGrip]             = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	    colors[ImGuiCol_ResizeGripActive]       = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_Tab]                    = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	    colors[ImGuiCol_TabHovered]             = ImVec4(0.352f, 0.352f, 0.352f, 1.000f);
	    colors[ImGuiCol_TabActive]              = ImVec4(0.295f, 0.295f, 0.295f, 1.000f);
	    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.098f, 0.098f, 0.098f, 1.000f);
	    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	    colors[ImGuiCol_DockingPreview]         = ImVec4(h[0], h[1], h[2], 0.781f);
	    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	    colors[ImGuiCol_PlotLines]              = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	    colors[ImGuiCol_DragDropTarget]         = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_NavHighlight]           = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(h[0], h[1], h[2], 1.000f);
	    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

	    style.ChildRounding = 4.0f;
	    style.FrameBorderSize = 1.0f;
	    style.FrameRounding = 2.0f;
	    style.GrabMinSize = 7.0f;
	    style.PopupRounding = 2.0f;
	    style.ScrollbarRounding = 12.0f;
	    style.ScrollbarSize = 13.0f;
	    style.TabBorderSize = 1.0f;
	    style.TabRounding = 0.0f;
	    style.WindowRounding = 4.0f;

		// TODO do gama correction in post process
		for(auto& c: style.Colors) {
			float exp = 2.2f;
			c = ImVec4(Math::FPow(c.x, exp), Math::FPow(c.y, exp), Math::FPow(c.z, exp), c.w);
		}
    }

	void ImGuiConfig() {
		// imgui
		float scaleX, scaleY;
		Engine::Context()->Window()->GetWindowContentScale(&scaleX, &scaleY);
		float contentScale = fmaxf(1.0f, fmaxf(scaleX, scaleY));

		// load font for imgui
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigWindowsMoveFromTitleBarOnly = true;

		File::FPath fontPath = Engine::AssetLoader::AssetPath();
		fontPath.append(Engine::GetConfig().DefaultFontPath);

		io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), contentScale * 16, nullptr, nullptr);
		ASSERT(io.Fonts->Build(), "Failed to build fonts");
		io.IniFilename = nullptr; // Do not save settings

		// Load imgui.ini
		String imguiFile{ PROJECT_CONFIG };
		imguiFile.append("imgui.ini");
		ImGui::LoadIniSettingsFromDisk(imguiFile.c_str());

		SetImGuiStyle();
	}
	
	XXEditor::XXEditor(Engine::XXEngine* engine){
		m_Engine = engine;
		Engine::Context()->Renderer()->InitUIPass(ImGuiConfig);
		Editor::EditorTimer::Initialize();
		EngineAssetMgr::Initialize();
		ProjectAssetMgr::Initialize();
		EditorUIMgr::Initialize();
		EditorLevelMgr::Initialize();
		m_UIController.reset(new UIController);
	}

	XXEditor::~XXEditor() {
		Engine::Context()->Renderer()->WaitGPU();
		Editor::EditorTimer::Release();
		EngineAssetMgr::Release();
		ProjectAssetMgr::Release();
		EditorUIMgr::Release();
		EditorLevelMgr::Release();
	}

	void XXEditor::EditorRun(){

		while (true) {
			EditorTimer::Instance()->Tick();
			EditorUIMgr::Instance()->Tick();
			if (!m_Engine->Tick()) {
				return;
			}
		}
	}
}
