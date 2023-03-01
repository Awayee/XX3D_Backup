#pragma once
#include "Core/File/CoreFile.h"

namespace Engine {
	inline void LoadShaderFile(const char* file, TVector<char>& code) {
		char shaderPath[128];
		strcpy(shaderPath, SHADER_PATH);
		strcat(shaderPath, file);
		File::LoadFileCode(shaderPath, code);
	}	
}