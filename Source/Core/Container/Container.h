#pragma once
#include <vector>
#include <unordered_map>
#include <map>
#include "TArray.h"


struct cmp {
	bool operator()(const char* s1, const char* s2) const {
		return std::strcmp(s1, s2) == 0;
	}
};

struct hs {
	size_t operator()(const char* s) const {
		unsigned long n = 0;
		for (; *s; s++) {
			n = 5 * n + *s;
		}
		return (size_t)n;
	}
};
template<class T>
using StrDict = std::unordered_map<const char*, T, hs, cmp>;
