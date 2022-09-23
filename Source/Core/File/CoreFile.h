#pragma once
#include <fstream>
#include <vector>
#include "../macro.h"
namespace File {
	void LoadFile(const char* fileName, std::vector<char>& code) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);
		ASSERT(file.is_open(), "file load failed!");

		size_t fileSize = (size_t)file.tellg();
		code.resize(fileSize);
		file.seekg(0);
		file.read(code.data(), fileSize);
		file.close();
	}
}