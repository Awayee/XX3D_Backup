#pragma once
#include "Core/File/CoreFile.h"

enum ERHIType {
	RHI_Vulkan,
	RHI_DX11,
	RHI_DX12,
	RHI_GL,
	RHI_NONE
};

enum ERenderPath {
	RENDER_FORWARD,
	RENDER_DEFERRED,
	RENDER_NONE
};

class ConfigManager {
private:
	FPath m_ConfigPath;
	FPath m_DefaultFontPath;
	ERHIType m_RHIType;
	ERenderPath m_RenderPath;
public:
	ConfigManager(const char* configPath);
	~ConfigManager();
	const FPath& GetAssetPath() { return m_ConfigPath; };
	const FPath& GetDefaultFontPath() { return m_DefaultFontPath; }
	ERHIType GetRHIType() { return m_RHIType; }
	ERenderPath GetRenderPath() { return m_RenderPath; }
};

ConfigManager* GetConfig();
