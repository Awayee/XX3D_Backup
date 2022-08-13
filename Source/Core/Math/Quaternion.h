#pragma once
#include "Vector.h"
namespace MATH {
	class Quaternion {
	public:
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 1.0f };
		Quaternion() = default;
		Quaternion(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {};
        static Quaternion AngleAxis(float a, const Vector3& axis) {
            Quaternion q;
            q.FromAngleAxis(a, axis);
            return q;
        }
		void FromAngleAxis(float a, const Vector3& axis) {
            float half_a(0.5 * a);
            float sin_v = std::sin(half_a);
            w = std::cos(half_a);
            x = axis.x * sin_v;
            y = axis.y * sin_v;
            z = axis.z * sin_v;
        }
        Vector3 RotateVector3(const Vector3& v) const {
            // nVidia SDK implementation
            Vector3 uv, uuv;
            Vector3 qvec(x, y, z);
            uv = qvec.Cross(v);
            uuv = qvec.Cross(uv);
            uv *= (2.0f * w);
            uuv *= 2.0f;

            return v + uv + uuv;
        }
        Quaternion Inverse() const // apply to non-zero quaternion
        {
            float norm = w * w + x * x + y * y + z * z;
            if (norm > 0.0)
            {
                float inv_norm = 1.0f / norm;
                return Quaternion(w * inv_norm, -x * inv_norm, -y * inv_norm, -z * inv_norm);
            }
            else
            {
                // return an invalid result to flag the error
                return Quaternion{};
            }
        }
        Vector3 Quaternion::operator*(const Vector3& v) const
        {
            // nVidia SDK implementation
            Vector3 uv, uuv;
            Vector3 qvec(x, y, z);
            uv = qvec.Cross(v);
            uuv = qvec.Cross(uv);
            uv *= (2.0f * w);
            uuv *= 2.0f;

            return v + uv + uuv;
        }
        Quaternion operator*(const Quaternion& rhs) const
        {
            return Quaternion(w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
                w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
                w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
                w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x);
        }
	};
}
