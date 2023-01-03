#pragma once
#include <fstream>
#include <filesystem>
#include "Core/Container/Container.h"

typedef std::filesystem::path FPath;

void LoadFileCode(const char* filePath, TVector<char>& code);

// lod .ini file
void LoadIniFile(const FPath& filePath, TUnorderedMap<FString, FString>& configMap);
