#include "Config.h"
#include <memory>
#include <string>
#include <mutex>

#include "Core/macro.h"
#include "Core/Json/CoreJson.h"
#include "Core/Container/Container.h"
#include "../ResourcesMacro.h"

#define ENGINE_CONFIG_PATH "Engine/EngineConfig.ini"

inline ERHIType ParseRHIType(const std::string& rhiTypeStr) {
	if ("Vulkan" == rhiTypeStr) {
		return RHI_Vulkan;
	}
	if ("DX11" == rhiTypeStr) {
		return RHI_DX11;
	}
	if ("DX12" == rhiTypeStr) {
		return RHI_DX12;
	}
	if ("GL" == rhiTypeStr) {
		return RHI_GL;
	}
	return RHI_NONE;
}

inline ERenderPath ParseRenderPath(const std::string& renderPathStr) {
	if("Deferred" == renderPathStr) {
		return RENDER_DEFERRED;
	}
	if("Forward" == renderPathStr) {
		return RENDER_FORWARD;
	}
	return RENDER_NONE;
}

ConfigManager::ConfigManager(const char* path) {
	// get asset root path firstly
	m_ConfigPath = CONFIG_PATH;
	FPath configPath = m_ConfigPath / path;
	TUnorderedMap<String, String> configMap;
	LoadIniFile(configPath, configMap);
	m_DefaultFontPath = m_ConfigPath / configMap["DefaultFont"];
	m_RHIType = ParseRHIType(configMap["RHIType"]);
	m_RenderPath = ParseRenderPath(configMap["RenderPath"]);
}

ConfigManager::~ConfigManager()
{
}


ConfigManager* GetConfig() {
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
