#pragma once
#include <fstream>
#include <filesystem>
#include "Core/Container/Container.h"
#include "Core/String/String.h"

typedef std::filesystem::path FPath;

void LoadFileCode(const char* filePath, TVector<char>& code);

// lod .ini file
void LoadIniFile(const FPath& filePath, TUnorderedMap<String, String>& configMap);

inline String JoinFilePath(const char* parentPath, const char* path) {
	String p(parentPath);
	p.append(path);
	return p;
}
