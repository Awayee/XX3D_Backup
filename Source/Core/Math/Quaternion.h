#pragma once
#include "Vector.h"
namespace Math {
	MATH_GENERIC class Quaternion {
	public:
		T x{ 0.0f };
		T y{ 0.0f };
		T z{ 0.0f };
		T w{ 1.0f };
		Quaternion() = default;
		Quaternion(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z), w(_w) {};
        void FromAngleAxis(T a, const Vector3<T>& axis);
        static Quaternion<T> AngleAxis(T a, const Vector3<T>& axis);
        Vector3<T> RotateFVector3(const Vector3<T>& v) const;
        Quaternion<T> Inverse() const; // apply to non-zero quaternion
        Vector3<T> operator*(const Vector3<T>& v) const;
		Quaternion<T> operator*(const Quaternion<T>& rhs) const;
	};
}
