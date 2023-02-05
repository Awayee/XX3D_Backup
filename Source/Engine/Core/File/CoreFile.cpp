#include "CoreFile.h"
#include "../macro.h"
void LoadFileCode(const char* filePath,TVector<char>& code) {
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	ASSERT(file.is_open(), "file load failed!");

	uint32 fileSize = (uint32)file.tellg();
	code.resize(fileSize);
	file.seekg(0);
	file.read(code.data(), fileSize);
	file.close();
}

// lod .ini file

void LoadIniFile(const FPath& filePath, TUnorderedMap<String, String>& configMap) {
	std::ifstream configFile(filePath);
	String fileLine;
	configMap.clear();
	while (std::getline(configFile, fileLine)) {
		uint32 separate = fileLine.find_first_of('=');
		if (separate > 0 && separate < fileLine.length() - 1) {
			String name = fileLine.substr(0, separate);
			String value = fileLine.substr(separate + 1, fileLine.length() - separate - 1);
			configMap.insert({ std::move(name), std::move(value) });
		}
	}
}

void LoadShaderFile(const char* path, TVector<char>& code)
{
	char shaderPath[128];
	strcpy(shaderPath, SHADER_PATH);
	strcat(shaderPath, path);
	LoadFileCode(shaderPath, code);
}
