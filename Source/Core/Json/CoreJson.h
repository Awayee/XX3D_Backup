#pragma once
#include <document.h>
#include <fstream>
#include <string>
#include "../logger.h"
namespace Json {
	bool ParseFile(const char* file, rapidjson::Document& doc) {
		std::ifstream in(file);
		if (!in.is_open()) {
			LOG_S("Load json file failed: %s", file);
			return false;
		}
		std::string fileContent{ std::istream_iterator<char>(in), std::istream_iterator<char>() };
		in.close();
		return !doc.Parse(fileContent.c_str()).HasParseError();
	}
}