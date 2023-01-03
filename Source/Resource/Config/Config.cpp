#include "Config.h"
#include <memory>
#include <string>
#include <mutex>

#include "Core/macro.h"
#include "Core/Json/CoreJson.h"
#include "Core/Container/Container.h"
#include "../ResourcesMacro.h"

#define ENGINE_CONFIG_PATH "Engine/EngineConfig.ini"

namespace Resource {

	ConfigManager::ConfigManager(const char* path) {
		// get asset root path firstly
		m_AssetPath = ASSETS_PATH;
		FPath configPath = m_AssetPath / path;
		TUnorderedMap<FString, FString> configMap;
		LoadIniFile(configPath, configMap);

		m_DefaultFontPath = m_AssetPath / configMap["DefaultFont"];

		// parse rhi type
		RHIType rhiType = RHI_None;
		std::string& rhiTypeStr = configMap["RHIType"];
		if("Vulkan" == rhiTypeStr) {
			rhiType = RHI_Vulkan;
		}
		else if ("DX11" == rhiTypeStr) {
			rhiType = RHI_DX11;
		}
		else if ("DX12" == rhiTypeStr) {
			rhiType = RHI_DX12;
		}
		else if ("GL" == rhiTypeStr) {
			rhiType = RHI_GL;
		}
		m_RHIType = rhiType;
	}

	ConfigManager::~ConfigManager()
	{
	}


	ConfigManager* GetConfigManager() {
		static std::unique_ptr<ConfigManager> s_ConfigManager;
		static std::mutex s_ConfigManagerMutex;
		if(nullptr == s_ConfigManager) {
			std::lock_guard<std::mutex> lock(s_ConfigManagerMutex);
			if(nullptr == s_ConfigManager) {
				s_ConfigManager.reset(new ConfigManager(ENGINE_CONFIG_PATH));
			}
		}
		return s_ConfigManager.get();
	}

}