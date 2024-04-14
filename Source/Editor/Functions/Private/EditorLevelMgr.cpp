#include "Functions/Public/EditorLevelMgr.h"
#include "Resource/Public/Config.h"
#include "Functions/Public/AssetManager.h"

namespace Editor {

	EditorLevelMgr::EditorLevelMgr() {
		File::FPath path = Engine::GetConfig().StartLevel;
		Engine::ALevelAsset* asset;
		auto node = ProjectAssetMgr::Instance()->GetFile(path);
		if(node) {
			asset = node->GetAsset<Engine::ALevelAsset>();
			LOG("Loaded start level: %s", path.string().c_str());
			LoadLevel(asset, path);
		}
		else {
			m_TempLevel.reset(new Engine::ALevelAsset);
			asset = m_TempLevel.get();
			LoadLevel(asset, "");
		}
	}

	EditorLevelMgr::~EditorLevelMgr() {
	}

	void EditorLevelMgr::LoadLevel(Engine::ALevelAsset* asset, const File::FPath& path) {
		m_LevelAsset = asset;
		m_LevelPath = path;
		m_Level.reset(new EditorLevel(*m_LevelAsset, Engine::RenderScene::GetDefaultScene()));
	}

	void EditorLevelMgr::SetLevelPath(File::PathStr path) {
		m_LevelPath = path;
	}

	void EditorLevelMgr::ReloadLevel() {
		FileNode* node = ProjectAssetMgr::Instance()->GetFile(m_LevelPath);
		if(node) {
			m_LevelAsset = node->GetAsset<Engine::ALevelAsset>();
			m_Level.reset(new EditorLevel(*m_LevelAsset, Engine::RenderScene::GetDefaultScene()));
		}
		else {
			m_Level.reset();
		}
	}

	bool EditorLevelMgr::SaveLevel() {
		if(m_LevelPath.empty()) {
			return false;
		}
		m_Level->SaveAsset(m_LevelAsset);
		Engine::AssetLoader::SaveProjectAsset(m_LevelAsset, m_LevelPath.string().c_str());
		LOG("[EditorLevelMgr::SaveLevel] Level saved: %s", m_LevelPath.string().c_str());
		return true;
	}

	EditorLevel* EditorLevelMgr::GetLevel() {
		return m_Level.get();
	}

	Engine::ALevelAsset* EditorLevelMgr::GetLevelAsset() {
		return m_LevelAsset;
	}

	void EditorLevelMgr::SetSelected(uint32 idx) {
		m_SelectIndex = idx;
	}

	uint32 EditorLevelMgr::GetSelected() {
		return m_SelectIndex;
	}
}
