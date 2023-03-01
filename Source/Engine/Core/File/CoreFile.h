#pragma once
#include <fstream>
#include <filesystem>
#include "Core/Container/Container.h"
#include "Core/Container/String.h"

namespace File {
	typedef std::filesystem::path FPath;
	void LoadFileCode(const char* file, TVector<char>& code);
	bool LoadIniFile(const char* file, TUnorderedMap<String, String>& configMap);
}

