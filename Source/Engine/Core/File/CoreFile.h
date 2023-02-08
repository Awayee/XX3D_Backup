#pragma once
#include <fstream>
#include <filesystem>
#include <stb_image.h>
#include "Core/Container/Container.h"
#include "Core/String/String.h"

typedef std::filesystem::path FPath;

inline String JoinAssetPath(const char* file) {
	String p(ASSETS_PATH);
	p.append(file);
	return p;
}

void LoadFileCode(const char* file, TVector<char>& code);

// lod .ini file
void LoadIniFile(const char* file, TUnorderedMap<String, String>& configMap);

void LoadShaderFile(const char* file, TVector<char>& code);

// image
float* LoadAssetImage(const char* file, int* w, int* h, int* n, int channels);
void FreeImage(void* data);
