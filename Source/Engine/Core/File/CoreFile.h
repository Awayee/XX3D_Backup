#pragma once
#include <fstream>
#include <filesystem>
#include "Core/Container/Container.h"
#include "Core/Container/String.h"

typedef std::filesystem::path FPath;

void LoadFileCode(const char* file, TVector<char>& code);

// lod .ini file
bool LoadIniFile(const char* file, TUnorderedMap<String, String>& configMap);

void LoadShaderFile(const char* file, TVector<char>& code);
