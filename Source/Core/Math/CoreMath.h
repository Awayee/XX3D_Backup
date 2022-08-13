#pragma once
#include <glm/glm.hpp>
#include "Vector.h"
#include "Quaternion.h"
namespace MATH {
	const float PI = 3.14159265358979323846264338327950288f;
	//const float Deg2Rad = PI / 180.0f;
	const float Deg2Rad = 0.017453292519943295f;
	//const float Rad2Deg = 180.0f / PI;
	const float Rad2Deg = 57.29577951308232f;

	int Min(int a, int b);
	bool IsNaN(float f) { return std::isnan(f); };
	bool FloatEqual(float a, float b, float precision = 0.0001f) { return std::fabs(b - a) <= precision; }

	// rad is default
	float ToRad(const float& f) { return f * Deg2Rad; }
	float ToDeg(const float& f) { return f * Rad2Deg; }
#ifdef MATH_DEG
#define TRI_FUNC(func, f){ return func(f * Deg2Rad); }
#define ATRI_FUNC(func, f) { return func(f) * Deg2Rad); }
#else
#define TRI_FUNC(func, f){ return func(f);}
#define ATRI_FUNC(func, ...) { return func( ##__VA_ARGS__); }
#endif
	float Sin(const float& f)TRI_FUNC(std::sin, f);
	float Cos(const float& f)TRI_FUNC(std::cos, f);
	float Tan(const float& f)TRI_FUNC(std::tan, f);
	float ASin(const float& f)ATRI_FUNC(std::asin, f);
	float ACos(const float& f)ATRI_FUNC(std::acos, f);
	float ATan(const float& f)ATRI_FUNC(std::atan, f);
	float ATan2(const float& a, const float& b)ATRI_FUNC(std::atan2, a, b);



	// Matrices
	class Matrix3x3 {

	};

	class Matrix4x4 {

	};
}
