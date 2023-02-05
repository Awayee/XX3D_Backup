#pragma once
#include <fstream>
#include <filesystem>
#include "Core/Container/Container.h"
#include "Core/String/String.h"

typedef std::filesystem::path FPath;

inline String JoinAssetPath(const char* path) {
	String p(ASSETS_PATH);
	p.append(path);
	return p;
}

void LoadFileCode(const char* filePath, TVector<char>& code);

// lod .ini file
void LoadIniFile(const char* filePath, TUnorderedMap<String, String>& configMap);

void LoadShaderFile(const char* path, TVector<char>& code);
