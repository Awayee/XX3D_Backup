#include <algorithm>
#include <cmath>
#include "Vector.h"
#include "Core/macro.h"

namespace MATH {
	float Vector2::operator[](size_t i) const
    {
        ASSERT(i < 2);
        return (i == 0 ? x : y);
    }
    float& Vector2::operator[](size_t i)
    {
        ASSERT(i < 2);
        return (i == 0 ? x : y);
    }
    float Vector2::Length() const
    {
        return std::hypot(x, y);
    }
    float Vector2::Normalize()
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
    Vector2 Vector2::Max(const Vector2& v0, const Vector2& v1)
    {
        return { std::max<float>(v0.x, v1.x), std::max<float>(v0.y, v1.y) };
    }
    Vector2 Vector2::Min(const Vector2& v0, const Vector2& v1)
    {
        return { std::min<float>(v0.x, v1.x), std::min<float>(v0.y, v1.y) };
    }
    float Vector3::operator[](size_t i) const
    {
        ASSERT(i < 3);
        return *(&x + i);
    }
    float& Vector3::operator[](size_t i)
    {
        ASSERT(i < 3);
        return *(&x + i);
    }
    Vector3 Vector3::operator/(float scalar) const
    {
        ASSERT(scalar != 0.0);
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    Vector3 Vector3::operator/(const Vector3& rhs) const
    {
        ASSERT((rhs.x != 0 && rhs.y != 0 && rhs.z != 0));
        return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
    }
    Vector3& Vector3::operator/=(float scalar)
    {
        ASSERT(scalar != 0.0);
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    Vector3& Vector3::operator/=(const Vector3& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
    Vector3 operator/(float scalar, const Vector3& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
        return Vector3(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
    }
    float Vector3::Length() const
    {
        return std::hypot(x, y, z);
    }
    void Vector3::Normalize()
    {
        float length = std::hypot(x, y, z);
        if (length == 0.f)
            return;

        float inv_lengh = 1.0f / length;
        x *= inv_lengh;
        y *= inv_lengh;
        z *= inv_lengh;
    }
    Vector3 Vector3::Max(const Vector3& v0, const Vector3& v1)
    {
        return { std::max<float>(v0.x, v1.x), std::max<float>(v0.y, v1.y), std::max<float>(v0.z, v1.z) };
    }
    Vector3 Vector3::Min(const Vector3& v0, const Vector3& v1)
    {
        return { std::min<float>(v0.x, v1.x), std::min<float>(v0.y, v1.y), std::min<float>(v0.z, v1.z) };
    }
    float Vector4::operator[](size_t i) const
    {
        ASSERT(i < 4);
        return *(&x + i);
    }
    float& Vector4::operator[](size_t i)
    {
        ASSERT(i < 4);
        return *(&x + i);
    }
    Vector4 Vector4::operator/(float scalar) const
    {
        ASSERT(scalar != 0.0);
        return Vector4(x / scalar, y / scalar, z / scalar, w / scalar);
    }
    Vector4 Vector4::operator/(const Vector4& rhs) const
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
        return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
    }
    Vector4 operator/(float scalar, const Vector4& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0 && rhs.w != 0);
        return Vector4(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
    }
    Vector4& Vector4::operator/=(float scalar)
    {
        ASSERT(scalar != 0.0);
        x /= scalar;
        y /= scalar;
        z /= scalar;
        w /= scalar;
        return *this;
    }
    Vector4& Vector4::operator/=(const Vector4& rhs)
    {
        ASSERT(rhs.x != 0 && rhs.y != 0 && rhs.z != 0);
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }
    Vector4 Vector4::Max(const Vector4& v0, const Vector4& v1)
    {
        return { std::max<float>(v0.x, v1.x), std::max<float>(v0.y, v1.y), std::max<float>(v0.z, v1.z), std::max<float>(v0.w, v1.w)};
    }
    Vector4 Vector4::Min(const Vector4& v0, const Vector4& v1)
    {
        return { std::min<float>(v0.x, v1.x), std::min<float>(v0.y, v1.y), std::min<float>(v0.z, v1.z), std::min<float>(v0.w, v1.w)};
    }
}