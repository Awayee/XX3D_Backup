#include "Assets.h"

#define PARSE_ASSETS_FILE(f)\
	char __s[128]; strcpy(__s, PROJECT_ASSETS); strcat(__s, f); f=__s

#define PARSE_ENGINE_ASSETS_FILE(f)\
	char __s[128]; strcpy(__s, ENGINE_ASSETS); strcat(__s, f); f=__s

#define REGISTER_ASSET_TYPE(cls) template cls AssetMgr::LoadAsset(const String& file); template cls AssetMgr::LoadEngineAsset(const String& file)


template <typename T> T AssetMgr::LoadAsset(const String& file) {
	const char* filePtr = file.c_str();
	PARSE_ASSETS_FILE(filePtr);
	T asset;
	asset.Load(filePtr);
	return asset; // will be destructed in debug
}


template <typename T> T AssetMgr::LoadEngineAsset(const String& file) {
	const char* filePtr = file.c_str();
	PARSE_ENGINE_ASSETS_FILE(filePtr);
	T asset;
	asset.Load(filePtr);
	return asset;
}


REGISTER_ASSET_TYPE(AImageAsset);
REGISTER_ASSET_TYPE(AFontAsset);
REGISTER_ASSET_TYPE(AExternalMeshAsset);

