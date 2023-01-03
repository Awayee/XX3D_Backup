#include "../Public/Editor.h"
#include "Core/macro.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <stb_image.h>
#include "Resource/Config/Config.h"

namespace Editor {

	XXEditor::XXEditor(Engine::XXEngine* engine)
	{
		m_Engine = engine;
		m_EditorUI.reset(new EditorUI(engine->GetWindowSystem()));
		engine->GetRenderSystem()->InitUIPass(static_cast<Engine::UIBase*>(m_EditorUI.get()));
	}

	XXEditor::~XXEditor() {
		
	}
	void XXEditor::EditorRun()
	{
		while(true) {
			if(!m_Engine->Tick()) {
				return;
			}
		}
	}
}