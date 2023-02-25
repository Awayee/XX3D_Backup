#pragma once
#include <string>

#define PARSE_ASSET_PATH(x)\
	char fullPath[128] = PROJECT_ASSETS;\
	strcat(fullPath, x);\
	x = fullPath

#define PARSE_ASSET_PATH_STRING(x)\
	x.