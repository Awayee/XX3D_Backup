#include "CoreFile.h"
#include "../macro.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define PARSE_ASSETS_FILE(f)\
	char __s[128]; strcpy(__s, ASSETS_PATH); strcat(__s, f); f=__s

void LoadFileCode(const char* file,TVector<char>& code) {
	std::ifstream f(file, std::ios::ate | std::ios::binary);
	ASSERT(f.is_open(), "file load failed!");

	uint32 fileSize = (uint32)f.tellg();
	code.resize(fileSize);
	f.seekg(0);
	f.read(code.data(), fileSize);
	f.close();
}

// lod .ini file

void LoadIniFile(const char* file, TUnorderedMap<String, String>& configMap) {
	std::ifstream configFile(file);
	String fileLine;
	configMap.clear();
	while (std::getline(configFile, fileLine)) {
		if(fileLine.empty() || fileLine[0] == '#') {
			continue;
		}
		uint32 separate = fileLine.find_first_of('=');
		if (separate > 0 && separate < fileLine.length() - 1) {
			String name = fileLine.substr(0, separate);
			String value = fileLine.substr(separate + 1, fileLine.length() - separate - 1);
			configMap.insert({ std::move(name), std::move(value) });
		}
	}
}

void LoadShaderFile(const char* file, TVector<char>& code){
	char shaderPath[128];
	strcpy(shaderPath, SHADER_PATH);
	strcat(shaderPath, file);
	LoadFileCode(shaderPath, code);
}

unsigned char* LoadAssetImage(const char* file, int* w, int* h, int* n, int channels){
	PARSE_ASSETS_FILE(file);
	return stbi_load(file, w, h, n, channels);
}

void FreeImage(void* data){
	stbi_image_free(data);
}
