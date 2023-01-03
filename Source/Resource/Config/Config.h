#pragma once
#include "Core/File/CoreFile.h"

namespace Resource {

	enum RHIType {
		RHI_Vulkan,
		RHI_DX11,
		RHI_DX12,
		RHI_GL,
		RHI_None
	};

	class ConfigManager {
	private:
		FPath m_AssetPath;
		FPath m_DefaultFontPath;
		RHIType m_RHIType;
	public:
		ConfigManager(const char* configPath);
		~ConfigManager();
		const FPath& GetAssetPath() { return m_AssetPath; };
		const FPath& GetDefaultFontPath() { return m_DefaultFontPath; }
		RHIType GetRHIType() { return m_RHIType; }
	};

	ConfigManager* GetConfigManager();
}