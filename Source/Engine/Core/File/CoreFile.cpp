#include "CoreFile.h"
#include "../macro.h"

#define PARSE_ASSETS_FILE(f)\
	char __s[128]; strcpy(__s, PROJECT_ASSETS); strcat(__s, f); f=__s

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

bool LoadIniFile(const char* file, TUnorderedMap<String, String>& configMap) {
	std::ifstream configFile(file);
	if(!configFile.is_open()) {
		return false;
	}
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
	return true;
}

void LoadShaderFile(const char* file, TVector<char>& code){
	char shaderPath[128];
	strcpy(shaderPath, SHADER_PATH);
	strcat(shaderPath, file);
	LoadFileCode(shaderPath, code);
}
