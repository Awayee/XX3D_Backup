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

inline EGPUType ParseGPUType(const std::string& gpuType) {
	if("Integrated" == gpuType) {
		return GPU_INTEGRATED;
	}
	if("Discrete" == gpuType) {
		return GPU_DISCRETE;
	}
	return GPU_NONE;
}

ConfigManager::ConfigManager(const char* file) {
	TUnorderedMap<String, String> configMap;
	LoadIniFile(file, configMap);
	m_DefaultFontPath = JoinAssetPath(configMap["DefaultFont"].c_str());
	m_RHIType = ParseRHIType(configMap["RHIType"]);
	m_RenderPath = ParseRenderPath(configMap["RenderPath"]);
	m_GpuType = ParseGPUType(configMap["PreferredGPU"]);
}

ConfigManager::~ConfigManager(){}


ConfigManager* GetConfig() {
	static std::unique_ptr<ConfigManager> s_ConfigManager;
	static std::mutex s_ConfigManagerMutex;
	if(nullptr == s_ConfigManager) {
		std::lock_guard<std::mutex> lock(s_ConfigManagerMutex);
		if(nullptr == s_ConfigManager) {
			String cfgFile = CONFIG_PATH;
			cfgFile.append(ENGINE_CONFIG_PATH);
			s_ConfigManager.reset(new ConfigManager(cfgFile.c_str()));
		}
	}
	return s_ConfigManager.get();
}
