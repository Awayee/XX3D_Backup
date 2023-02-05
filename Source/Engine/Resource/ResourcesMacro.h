#pragma once
#include <string>

#define PARSE_ASSET_PATH(x)\
	char fullPath[128] = ASSETS_PATH;\
	strcat(fullPath, x);\
	x = fullPath

#define PARSE_ASSET_PATH_STRING(x)\
	x.