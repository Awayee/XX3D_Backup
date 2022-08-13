#pragma once
#include <iostream>

#define LOG(arg)\
	std::cout<< arg << std::endl

#define LOG_S(...)\
	printf(##__VA_ARGS__);\
	std::cout << std::endl

#define ERROR(x)\
	throw std::runtime_error(x)

#define ASSERT(x, msg)\
	if(!x) ERROR(msg)