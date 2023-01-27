#include "../Public/Editor.h"
#include "Resource/Config/Config.h"
#include "Engine/Mesh/MeshUtil.h"

namespace Editor {
	void XXEditor::LoadMesh()
	{
		TVector<Engine::SPrimitiveData> primitives;
		Engine::LoadMeshFromGLTF("keqing/keqing.glb", primitives);
		m_Ms.reset(new Engine::RenderMesh(primitives, nullptr));
	}
	XXEditor::XXEditor(Engine::XXEngine* engine)
	{
		m_Engine = engine;
		m_EditorUI.reset(new EditorUI(engine->GetWindowSystem()));
		engine->GetRenderSystem()->InitUIPass(static_cast<Engine::UIBase*>(m_EditorUI.get()));

		// TODO TEST
		LoadMesh();
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