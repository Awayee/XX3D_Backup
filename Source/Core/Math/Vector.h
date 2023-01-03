#pragma once

namespace MATH {
    // Vectors
    class Vector2 {
    public:
        float x{ 0.f }, y{ 0.f };
        Vector2() = default;
        Vector2(float _x, float _y) : x(_x), y(_y) {}
        explicit Vector2(float scaler) : x(scaler), y(scaler) {}
        explicit Vector2(float* r) : x(r[0]), y(r[1]) {}
        float operator[](size_t i) const;
        float& operator[](size_t i);
        bool operator==(const Vector2& rhs) const { return (x == rhs.x && y == rhs.y); }
        bool operator!=(const Vector2& rhs) const { return (x != rhs.x || y != rhs.y); }
        // arithmetic operations
        Vector2 operator+(const Vector2& rhs) const { return Vector2(x + rhs.x, y + rhs.y); }

        Vector2 operator-(const Vector2& rhs) const { return Vector2(x - rhs.x, y - rhs.y); }

        Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }

        Vector2 operator*(const Vector2& rhs) const { return Vector2(x * rhs.x, y * rhs.y); }

        Vector2 operator/(float scale) const{
            float inv = 1.0f / scale;
            return Vector2(x * inv, y * inv);
        }

        Vector2 operator/(const Vector2& rhs) const { return Vector2(x / rhs.x, y / rhs.y); }

        const Vector2& operator+() const { return *this; }

        Vector2 operator-() const { return Vector2(-x, -y); }

        // overloaded operators to help Vector2
        friend Vector2 operator*(float scalar, const Vector2& rhs) { return Vector2(scalar * rhs.x, scalar * rhs.y); }

        friend Vector2 operator/(float fScalar, const Vector2& rhs) { return Vector2(fScalar / rhs.x, fScalar / rhs.y); }

        friend Vector2 operator+(const Vector2& lhs, float rhs) { return Vector2(lhs.x + rhs, lhs.y + rhs); }

        friend Vector2 operator+(float lhs, const Vector2& rhs) { return Vector2(lhs + rhs.x, lhs + rhs.y); }

        friend Vector2 operator-(const Vector2& lhs, float rhs) { return Vector2(lhs.x - rhs, lhs.y - rhs); }

        friend Vector2 operator-(float lhs, const Vector2& rhs) { return Vector2(lhs - rhs.x, lhs - rhs.y); }

        // arithmetic updates
        Vector2& operator+=(const Vector2& rhs)
        {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        Vector2& operator+=(float scalar)
        {
            x += scalar;
            y += scalar;

            return *this;
        }

        Vector2& operator-=(const Vector2& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        Vector2& operator-=(float scalar)
        {
            x -= scalar;
            y -= scalar;

            return *this;
        }

        Vector2& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;

            return *this;
        }

        Vector2& operator*=(const Vector2& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;

            return *this;
        }

        Vector2& operator/=(float scalar)
        {

            float inv = 1.0f / scalar;

            x *= inv;
            y *= inv;

            return *this;
        }

        Vector2& operator/=(const Vector2& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;

            return *this;
        }
        float Length() const;
        float SquaredLength() const { return x * x + y * y; }
        float Cross(const Vector2& rhs) const { return x * rhs.y - y * rhs.x; }
        float Dot(const Vector2& vec) const { return x * vec.x + y * vec.y; }
        float Distance(const Vector2& rhs) const { return (*this - rhs).Length(); }
        float Normalize();

        static Vector2 Max(const Vector2& v0, const Vector2& v1);
        static Vector2 Min(const Vector2& v0, const Vector2& v1);

    };

    class Vector3 {
    public:
        float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
        Vector3() = default;
        Vector3(float scalar) :x(scalar), y(scalar), z(scalar) {};
        Vector3(float _x, float _y, float _z) :x(_x), y(_y), z(_z) {};
        explicit Vector3(const float* coords) : x{ coords[0] }, y{ coords[1] }, z{ coords[2] } {}
        float operator[](size_t i) const;
        float& operator[](size_t i);
        bool operator==(const Vector3& rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z); }

        bool operator!=(const Vector3& rhs) const { return x != rhs.x || y != rhs.y || z != rhs.z; }

        // arithmetic operations
        Vector3 operator+(const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }

        Vector3 operator-(const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }

        Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }

        Vector3 operator*(const Vector3& rhs) const { return Vector3(x * rhs.x, y * rhs.y, z * rhs.z); }

        Vector3 operator/(float scalar) const;

        Vector3 operator/(const Vector3& rhs) const;

        const Vector3& operator+() const { return *this; }

        Vector3 operator-() const { return Vector3(-x, -y, -z); }

        // overloaded operators to help Vector3
        friend Vector3 operator*(float scalar, const Vector3& rhs)
        {
            return Vector3(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
        }

        friend Vector3 operator/(float scalar, const Vector3& rhs);

        friend Vector3 operator+(const Vector3& lhs, float rhs)
        {
            return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
        }

        friend Vector3 operator+(float lhs, const Vector3& rhs)
        {
            return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
        }

        friend Vector3 operator-(const Vector3& lhs, float rhs)
        {
            return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
        }

        friend Vector3 operator-(float lhs, const Vector3& rhs)
        {
            return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
        }

        // arithmetic updates
        Vector3& operator+=(const Vector3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        Vector3& operator+=(float scalar)
        {
            x += scalar;
            y += scalar;
            z += scalar;
            return *this;
        }

        Vector3& operator-=(const Vector3& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        Vector3& operator-=(float scalar)
        {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            return *this;
        }

        Vector3& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3& operator*=(const Vector3& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            return *this;
        }

        Vector3& operator/=(float scalar);

        Vector3& operator/=(const Vector3& rhs);

        float Length() const;

        float SquaredLength() const { return x * x + y * y + z * z; }

        float Distance(const Vector3& rhs) const { return (*this - rhs).Length(); }

        float SquaredDistance(const Vector3& rhs) const { return (*this - rhs).SquaredLength(); }

        float Dot(const Vector3& vec) const { return x * vec.x + y * vec.y + z * vec.z; }

        void Normalize();

        Vector3 Cross(const Vector3& rhs) const { return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x); }

        static Vector3 Max(const Vector3& v0, const Vector3& v1);

        static Vector3 Min(const Vector3& v0, const Vector3& v1);
    };


    class Vector4 {
    public:
        float x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f };

    public:
        Vector4() = default;
        Vector4(float _x, float _y, float _z, float _w) : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}
        Vector4(const Vector3 & v3, float w_) : x{ v3.x }, y{ v3.y }, z{ v3.z }, w{ w_ } {}

        explicit Vector4(float coords[4]) : x{ coords[0] }, y{ coords[1] }, z{ coords[2] }, w{ coords[3] } {}

        float operator[](size_t i) const;

        float& operator[](size_t i);

        Vector4& operator=(float scalar)
        {
            x = scalar;
            y = scalar;
            z = scalar;
            w = scalar;
            return *this;
        }

        bool operator==(const Vector4 & rhs) const { return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w); }

        bool operator!=(const Vector4 & rhs) const { return !(rhs == *this); }

        Vector4 operator+(const Vector4 & rhs) const { return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
        Vector4 operator-(const Vector4 & rhs) const { return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
        Vector4 operator*(float scalar) const { return Vector4(x * scalar, y * scalar, z * scalar, w * scalar); }
        Vector4 operator*(const Vector4 & rhs) const { return Vector4(rhs.x * x, rhs.y * y, rhs.z * z, rhs.w * w); }
        Vector4 operator/(float scalar) const;
        Vector4 operator/(const Vector4& rhs) const;

        const Vector4& operator+() const { return *this; }

        Vector4 operator-() const { return Vector4(-x, -y, -z, -w); }

        friend Vector4 operator*(float scalar, const Vector4 & rhs)
        {
            return Vector4(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w);
        }

        friend Vector4 operator/(float scalar, const Vector4& rhs);

        friend Vector4 operator+(const Vector4 & lhs, float rhs)
        {
            return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
        }

        friend Vector4 operator+(float lhs, const Vector4 & rhs)
        {
            return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
        }

        friend Vector4 operator-(const Vector4 & lhs, float rhs)
        {
            return Vector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
        }

        friend Vector4 operator-(float lhs, const Vector4 & rhs)
        {
            return Vector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
        }

        // arithmetic updates
        Vector4& operator+=(const Vector4 & rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }

        Vector4& operator-=(const Vector4 & rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
            return *this;
        }

        Vector4& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }

        Vector4& operator+=(float scalar)
        {
            x += scalar;
            y += scalar;
            z += scalar;
            w += scalar;
            return *this;
        }

        Vector4& operator-=(float scalar)
        {
            x -= scalar;
            y -= scalar;
            z -= scalar;
            w -= scalar;
            return *this;
        }

        Vector4& operator*=(const Vector4 & rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            w *= rhs.w;
            return *this;
        }

        Vector4& operator/=(float scalar);

        Vector4& operator/=(const Vector4& rhs);

        float Dot(const Vector4 & vec) const { return x * vec.x + y * vec.y + z * vec.z + w * vec.w; }

        static Vector4 Max(const Vector4& v0, const Vector4& v1);
        static Vector4 Min(const Vector4& v0, const Vector4& v1);
    };
}