#pragma once
#include "AssetType.h"

namespace Engine {
	class Assets {
	public:
		template<typename T> static T LoadAsset(const String& file);
		template<typename T> static T LoadEngineAsset(const String& file);
	};
}
