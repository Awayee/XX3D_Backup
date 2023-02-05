#pragma once
#include "EditorUI.h"
#include "Engine.h"
#include "Core/Memory/SmartPointer.h"
#include "Render/Mesh/RenderMesh.h"

namespace Editor {
	class XXEditor {
	private:
		// TODO TEST
		TUniquePtr<Engine::RenderMesh> m_Ms;
		Engine::XXEngine* m_Engine;
		TUniquePtr<EditorUI> m_EditorUI;
		Engine::RenderScene* m_MainScene;
		Engine::Camera* m_MainCamera;

		bool m_MousePressed = false;
		float m_LastX = 0.0f;
		float m_LastY = 0.0f;

	private:
		void LoadMesh();
		void RegisterEvents();
		void InitResources();
		void OnKeyEvent(Engine::EKeyCode keyCode, Engine::EInputType inputType);
		void OnMouseButtonEvent(Engine::EButtonCode btnCode, Engine::EInputType inputType);
		void OnMouseMoveEvent(float x, float y);
		void UpdateUI();
	public:
		XXEditor(Engine::XXEngine* engine);
		~XXEditor();
		void EditorRun();
	};
}
