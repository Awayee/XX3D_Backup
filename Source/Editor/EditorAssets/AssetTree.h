#pragma once
#include "Core/Container/Container.h"
#include "Core/Memory/SmartPointer.h"
#include "Core/Container/String.h"
#include "Resource/Assets/Assets.h"

namespace Editor {

	class AssetNode {
	private:
		String m_Path; // Relative path
	};

	class AssetData: public AssetNode {
		TUniquePtr<Engine::AAssetBase> m_Asset;
	};

	class AssetFolder: public AssetNode {
	};

	class AssetTree {
	private:
		
	};
}