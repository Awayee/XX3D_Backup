#include "ClientCode/Public/Client.h"
#include "Objects/Public/RenderScene.h"
#include "Asset/Public/AssetLoader.h"
#include "Asset/Public/LevelAsset.h"

namespace Runtime {

	Client::Client() {
		Engine::ALevelAsset asset;
		Engine::AssetLoader::LoadProjectAsset(&asset, "Level/test.level");
		m_Level.reset(new Engine::Level(asset, Engine::RenderScene::GetDefaultScene()));
	}

	void Client::Tick() {
	}

	Client::~Client() {
		m_Level.reset();
	}
}
