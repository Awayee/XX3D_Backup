#include <algorithm>
#include <cmath>
#include "Vector.h"
#include "Core/macro.h"

namespace Math {
    // vector2
    MATH_GENERIC T Vector2<T>::operator[](int i) const
    {
        ASSERT(i < 2);
        return (i == 0 ? x : y);
    }

    MATH_GENERIC T& Vector2<T>::operator[](int i)
    {
        ASSERT(i < 2);
        return (i == 0 ? x : y);
    }

    MATH_GENERIC float Vector2<T>::Length() const
    {
        return std::hypot(x, y);
    }

    MATH_GENERIC float Vector2<T>::Normalize()
    {
        float lengh = std::hypot(x, y);

        if (lengh > 0.0f)
        {
            float inv_length = 1.0f / lengh;
            x *= inv_length;
            y *= inv_length;
        }

        return lengh;
    }
    MATH_GENERIC Vector2<T> Vector2<T>::Max(const Vector2<T>& v0, const Vector2<T>& v1)
    {
        return { std::max<float>(v0.x, v1.x), std::max<float>(v0.y, v1.y) };
    }
    MATH_GENERIC Vector2<T> Vector2<T>::Min(const Vector2<T>& v0, const Vector2<T>& v1)
    {
        return { std::min<float>(v0.x, v1.x), std::min<float>(v0.y, v1.y) };
    }

    // vector3
    MATH_GENERIC T Vector3<T>::operator[](int i) const
    {
        ASSERT(i < 3);
        return *(&x + i);
    }
    MATH_GENERIC T& Vector3<T>::operator[](int i)
    {
        ASSERT(i < 3);
        return *(&x + i);
    }
    MATH_GENERIC Vector3<T> Vector3<T>::operator/(T scalar) const
    {
        ASSERT(scalar != 0.0);
        return Vector3<T>(x / scalar, y / scalar, z / scalar);
    }
    MATH_GENERIC Vector3<T> Vector3<T>::operator/(const Vector3<T>& rhs) const
    {
        ASSERT((rhs.x != 0 && rhs.y != 0 && rhs.z != 0));
        return Vector3<T>(x / rhs.x, y / rhs.y, z / rhs.z);
    }
    MATH_GENERIC Vector3<T>& Vector3<T>::operator/=(float scalar)
    {
        ASSERT(scalar != 0.0);
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    MATH_GENERIC Vector3<T>& Vector3<T>::operator/=(const Vector3<T>& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
    MATH_GENERIC Vector3<T> operator/(T scalar, const Vector3<T>& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
        return Vector3(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
    }
    MATH_GENERIC float Vector3<T>::Length() const
    {
        return std::hypot(x, y, z);
    }
    MATH_GENERIC void Vector3<T>::Normalize()
    {
        float length = std::hypot(x, y, z);
        if (length == 0.f)
            return;

        float inv_lengh = 1.0f / length;
        x *= inv_lengh;
        y *= inv_lengh;
        z *= inv_lengh;
    }
    MATH_GENERIC Vector3<T> Vector3<T>::Max(const Vector3<T>& v0, const Vector3<T>& v1)
    {
        return { std::max<float>(v0.x, v1.x), std::max<float>(v0.y, v1.y), std::max<float>(v0.z, v1.z) };
    }
    MATH_GENERIC Vector3<T> Vector3<T>::Min(const Vector3<T>& v0, const Vector3<T>& v1)
    {
        return { std::min<float>(v0.x, v1.x), std::min<float>(v0.y, v1.y), std::min<float>(v0.z, v1.z) };
    }

    // vector4
    MATH_GENERIC T Vector4<T>::operator[](int i) const
    {
        ASSERT(i < 4);
        return *(&x + i);
    }
    MATH_GENERIC T& Vector4<T>::operator[](int i)
    {
        ASSERT(i < 4);
        return *(&x + i);
    }
    MATH_GENERIC Vector4<T> Vector4<T>::operator/(T scalar) const
    {
        ASSERT(scalar != 0.0);
        return Vector4<T>(x / scalar, y / scalar, z / scalar, w / scalar);
    }
    MATH_GENERIC Vector4<T> Vector4<T>::operator/(const Vector4<T>& rhs) const
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
        return Vector4<T>(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
    }
    MATH_GENERIC Vector4<T> operator/(T scalar, const Vector4<T>& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
        return Vector4(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
    }
    MATH_GENERIC Vector4<T>& Vector4<T>::operator/=(T scalar)
    {
        ASSERT(scalar != 0.0);
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }
    MATH_GENERIC Vector4<T>& Vector4<T>::operator/=(const Vector4<T>& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }
    MATH_GENERIC Vector4<T> Vector4<T>::Max(const Vector4<T>& v0, const Vector4<T>& v1)
    {
        return { std::max<float>(v0.x, v1.x), std::max<float>(v0.y, v1.y), std::max<float>(v0.z, v1.z), std::max<float>(v0.w, v1.w)};
    }
    MATH_GENERIC Vector4<T> Vector4<T>::Min(const Vector4<T>& v0, const Vector4<T>& v1)
    {
        return { std::min<float>(v0.x, v1.x), std::min<float>(v0.y, v1.y), std::min<float>(v0.z, v1.z), std::min<float>(v0.w, v1.w)};
    }
}