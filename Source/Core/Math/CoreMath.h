#pragma once

#include <glm/glm.hpp>
#include "Vector.h"
#include "Quaternion.h"
#include "Matrix.h"

namespace MATH {
	const float PI = 3.14159265358979323846264338327950288f;
	//const float Deg2Rad = PI / 180.0f;
	const float Deg2Rad = 0.017453292519943295f;
	//const float Rad2Deg = 180.0f / PI;
	const float Rad2Deg = 57.29577951308232f;

	inline int Min(int a, int b) { return a < b ? a : b; };
	inline bool IsNaN(float f) { return std::isnan(f); };
	inline bool FloatEqual(float a, float b, float precision = 0.0001f) { return std::fabs(b - a) <= precision; }

	// rad is default
	inline float ToRad(const float& f) { return f * Deg2Rad; }
	inline float ToDeg(const float& f) { return f * Rad2Deg; }
#ifdef MATH_DEG
#define TRI_FUNC(func, f){ return func(f * Deg2Rad); }
#define ATRI_FUNC(func, f) { return func(f) * Deg2Rad); }
#else
#define TRI_FUNC(func, f){ return func(f);}
#define ATRI_FUNC(func, ...) { return func( ##__VA_ARGS__); }
#endif
	inline float Sin(const float& f)TRI_FUNC(std::sin, f);
	inline float Cos(const float& f)TRI_FUNC(std::cos, f);
	inline float Tan(const float& f)TRI_FUNC(std::tan, f);
	inline float ASin(const float& f)ATRI_FUNC(std::asin, f);
	inline float ACos(const float& f)ATRI_FUNC(std::acos, f);
	inline float ATan(const float& f)ATRI_FUNC(std::atan, f);
	inline float ATan2(const float& a, const float& b)ATRI_FUNC(std::atan2, a, b);
}