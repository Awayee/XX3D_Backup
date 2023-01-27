#include "Quaternion.h"
#include <cmath>
namespace Math {
	MATH_GENERIC void Quaternion<T>::FromAngleAxis(T a, const Vector3<T>& axis)
	{
        float half_a(0.5f * a);
        float sin_v = std::sin(half_a);
        w = std::cos(half_a);
        x = axis.x * sin_v;
        y = axis.y * sin_v;
        z = axis.z * sin_v;
	}
    MATH_GENERIC Quaternion<T> Quaternion<T>::AngleAxis(T a, const Vector3<T>& axis)
    {
        Quaternion<T> q;
        q.FromAngleAxis(a, axis);
        return q;
    }
    MATH_GENERIC Vector3<T> Quaternion<T>::RotateFVector3(const Vector3<T>& v) const
    {
        // nVidia SDK implementation
        Vector3<T> uv, uuv;
        Vector3<T> qvec(x, y, z);
        uv   = Vector3<T>::Cross(qvec, v);
        uuv  = Vector3<T>::Cross(qvec, uv);
        uv  *=  (2.0f * w);
        uuv *=  2.0f;

        return v + uv + uuv;
    }
    MATH_GENERIC Quaternion<T> Quaternion<T>::Inverse() const
    {
        float norm = w * w + x * x + y * y + z * z;
        if (norm > 0.0)
        {
            float inv_norm = 1.0f / norm;
            return Quaternion<T>(w * inv_norm, -x * inv_norm, -y * inv_norm, -z * inv_norm);
        }
        else
        {
            // return an invalid result to flag the error
            return Quaternion<T>{};
        }
    }
    MATH_GENERIC Vector3<T> Quaternion<T>::operator*(const Vector3<T>& v) const
    {
        // nVidia SDK implementation
        Vector3<T> uv, uuv;
        Vector3<T> qvec(x, y, z);
        uv = Vector3<T>::Cross(qvec, v);
        uuv = Vector3<T>::Cross(qvec, uv);
        uv *= (2.0f * w);
        uuv *= 2.0f;

        return v + uv + uuv;
    }
    MATH_GENERIC Quaternion<T> Quaternion<T>::operator*(const Quaternion<T>& rhs) const
    {
        return Quaternion<T>(w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
            w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x);
    }

    template Quaternion<float>;
    template Quaternion<double>;
}