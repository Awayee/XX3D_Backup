#pragma once
#include "Vector.h"
#include "Quaternion.h"

namespace MATH {
	class Matrix3x3 {
	public:
        static const Matrix3x3 IDENTITY;
		float m_mat[3][3];
		Matrix3x3() = default;
        explicit Matrix3x3(float arr[3][3]);

        Matrix3x3(float(&float_array)[9])
        {
            m_mat[0][0] = float_array[0];
            m_mat[0][1] = float_array[1];
            m_mat[0][2] = float_array[2];
            m_mat[1][0] = float_array[3];
            m_mat[1][1] = float_array[4];
            m_mat[1][2] = float_array[5];
            m_mat[2][0] = float_array[6];
            m_mat[2][1] = float_array[7];
            m_mat[2][2] = float_array[8];
        }
        Matrix3x3(float entry00,
            float entry01,
            float entry02,
            float entry10,
            float entry11,
            float entry12,
            float entry20,
            float entry21,
            float entry22)
        {
            m_mat[0][0] = entry00;
            m_mat[0][1] = entry01;
            m_mat[0][2] = entry02;
            m_mat[1][0] = entry10;
            m_mat[1][1] = entry11;
            m_mat[1][2] = entry12;
            m_mat[2][0] = entry20;
            m_mat[2][1] = entry21;
            m_mat[2][2] = entry22;
        }

        Matrix3x3(const Vector3& row0, const Vector3& row1, const Vector3& row2)
        {
            m_mat[0][0] = row0.x;
            m_mat[0][1] = row0.y;
            m_mat[0][2] = row0.z;
            m_mat[1][0] = row1.x;
            m_mat[1][1] = row1.y;
            m_mat[1][2] = row1.z;
            m_mat[2][0] = row2.x;
            m_mat[2][1] = row2.y;
            m_mat[2][2] = row2.z;
        }
        // assignment and comparison
        float* operator[](size_t row_index) const { return (float*)m_mat[row_index]; }
        bool operator==(const Matrix3x3& rhs) const
        {
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                for (size_t col_index = 0; col_index < 3; col_index++)
                {
                    if (m_mat[row_index][col_index] != rhs.m_mat[row_index][col_index])
                        return false;
                }
            }

            return true;
        }
        bool operator!=(const Matrix3x3& rhs) const { return !operator==(rhs); }

        // arithmetic operations
        Matrix3x3 operator+(const Matrix3x3& rhs) const
        {
            Matrix3x3 sum;
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                for (size_t col_index = 0; col_index < 3; col_index++)
                {
                    sum.m_mat[row_index][col_index] = m_mat[row_index][col_index] + rhs.m_mat[row_index][col_index];
                }
            }
            return sum;
        }

        Matrix3x3 operator-(const Matrix3x3& rhs) const
        {
            Matrix3x3 diff;
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                for (size_t col_index = 0; col_index < 3; col_index++)
                {
                    diff.m_mat[row_index][col_index] = m_mat[row_index][col_index] - rhs.m_mat[row_index][col_index];
                }
            }
            return diff;
        }

        Matrix3x3 operator*(const Matrix3x3& rhs) const
        {
            Matrix3x3 prod;
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                for (size_t col_index = 0; col_index < 3; col_index++)
                {
                    prod.m_mat[row_index][col_index] = m_mat[row_index][0] * rhs.m_mat[0][col_index] +
                        m_mat[row_index][1] * rhs.m_mat[1][col_index] +
                        m_mat[row_index][2] * rhs.m_mat[2][col_index];
                }
            }
            return prod;
        }
        // matrix * vector [3x3 * 3x1 = 3x1]
        Vector3 operator*(const Vector3& rhs) const
        {
            Vector3 prod;
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                prod[row_index] =
                    m_mat[row_index][0] * rhs.x + m_mat[row_index][1] * rhs.y + m_mat[row_index][2] * rhs.z;
            }
            return prod;
        }
        // matrix * scalar
        Matrix3x3 operator*(float scalar) const
        {
            Matrix3x3 prod;
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                for (size_t col_index = 0; col_index < 3; col_index++)
                    prod[row_index][col_index] = scalar * m_mat[row_index][col_index];
            }
            return prod;
        }

        // scalar * matrix
        friend Matrix3x3 operator*(float scalar, const Matrix3x3& rhs)
        {
            Matrix3x3 prod;
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                for (size_t col_index = 0; col_index < 3; col_index++)
                    prod[row_index][col_index] = scalar * rhs.m_mat[row_index][col_index];
            }
            return prod;
        }

        // utilities
        Matrix3x3 Transpose() const
        {
            Matrix3x3 transpose_v;
            for (size_t row_index = 0; row_index < 3; row_index++)
            {
                for (size_t col_index = 0; col_index < 3; col_index++)
                    transpose_v[row_index][col_index] = m_mat[col_index][row_index];
            }
            return transpose_v;
        }

        float Determinant() const
        {
            float cofactor00 = m_mat[1][1] * m_mat[2][2] - m_mat[1][2] * m_mat[2][1];
            float cofactor10 = m_mat[1][2] * m_mat[2][0] - m_mat[1][0] * m_mat[2][2];
            float cofactor20 = m_mat[1][0] * m_mat[2][1] - m_mat[1][1] * m_mat[2][0];

            float det = m_mat[0][0] * cofactor00 + m_mat[0][1] * cofactor10 + m_mat[0][2] * cofactor20;

            return det;
        }
        bool Inverse(Matrix3x3& inv_mat, float fTolerance = 1e-06) const;

        Matrix3x3 Inverse(float tolerance = 1e-06) const
        {
            Matrix3x3 inv;
            Inverse(inv, tolerance);
            return inv;
        }
	};

    inline void MatrixToQuaternion(const Matrix3x3& m, Quaternion& q);

    inline void QuaternionToMatrix(const Quaternion& q, Matrix3x3& m) {
        float fTx = q.x + q.x;   // 2x
        float fTy = q.y + q.y;   // 2y
        float fTz = q.z + q.z;   // 2z
        float fTwx = fTx * q.w; // 2xw
        float fTwy = fTy * q.w; // 2yw
        float fTwz = fTz * q.w; // 2z2
        float fTxx = fTx * q.x; // 2x^2
        float fTxy = fTy * q.x; // 2xy
        float fTxz = fTz * q.x; // 2xz
        float fTyy = fTy * q.y; // 2y^2
        float fTyz = fTz * q.y; // 2yz
        float fTzz = fTz * q.z; // 2z^2

       m[0][0] = 1.0f - (fTyy + fTzz); // 1 - 2y^2 - 2z^2
       m[0][1] = fTxy - fTwz;          // 2xy - 2wz
       m[0][2] = fTxz + fTwy;          // 2xz + 2wy
       m[1][0] = fTxy + fTwz;          // 2xy + 2wz
       m[1][1] = 1.0f - (fTxx + fTzz); // 1 - 2x^2 - 2z^2
       m[1][2] = fTyz - fTwx;          // 2yz - 2wx
       m[2][0] = fTxz - fTwy;          // 2xz - 2wy
       m[2][1] = fTyz + fTwx;          // 2yz + 2wx
       m[2][2] = 1.0f - (fTxx + fTyy); // 1 - 2x^2 - 2y^2
    }


    class Matrix4x4
    {
    public:
        /// The matrix entries, indexed by [row][col]
        float m_mat[4][4];

    public:
        /** Default constructor.
        @note
        It does <b>NOT</b> initialize the matrix for efficiency.
        */
        Matrix4x4() = default;

        Matrix4x4(const Matrix4x4& mat)
        {
            m_mat[0][0] = mat[0][0];
            m_mat[0][1] = mat[0][1];
            m_mat[0][2] = mat[0][2];
            m_mat[0][3] = mat[0][3];
            m_mat[1][0] = mat[1][0];
            m_mat[1][1] = mat[1][1];
            m_mat[1][2] = mat[1][2];
            m_mat[1][3] = mat[1][3];
            m_mat[2][0] = mat[2][0];
            m_mat[2][1] = mat[2][1];
            m_mat[2][2] = mat[2][2];
            m_mat[2][3] = mat[2][3];
            m_mat[3][0] = mat[3][0];
            m_mat[3][1] = mat[3][1];
            m_mat[3][2] = mat[3][2];
            m_mat[3][3] = mat[3][3];
        }

        Matrix4x4(const float (&float_array)[16])
        {
            m_mat[0][0] = float_array[0];
            m_mat[0][1] = float_array[1];
            m_mat[0][2] = float_array[2];
            m_mat[0][3] = float_array[3];
            m_mat[1][0] = float_array[4];
            m_mat[1][1] = float_array[5];
            m_mat[1][2] = float_array[6];
            m_mat[1][3] = float_array[7];
            m_mat[2][0] = float_array[8];
            m_mat[2][1] = float_array[9];
            m_mat[2][2] = float_array[10];
            m_mat[2][3] = float_array[11];
            m_mat[3][0] = float_array[12];
            m_mat[3][1] = float_array[13];
            m_mat[3][2] = float_array[14];
            m_mat[3][3] = float_array[15];
        }

        Matrix4x4(float m00,
                  float m01,
                  float m02,
                  float m03,
                  float m10,
                  float m11,
                  float m12,
                  float m13,
                  float m20,
                  float m21,
                  float m22,
                  float m23,
                  float m30,
                  float m31,
                  float m32,
                  float m33)
        {
            m_mat[0][0] = m00;
            m_mat[0][1] = m01;
            m_mat[0][2] = m02;
            m_mat[0][3] = m03;
            m_mat[1][0] = m10;
            m_mat[1][1] = m11;
            m_mat[1][2] = m12;
            m_mat[1][3] = m13;
            m_mat[2][0] = m20;
            m_mat[2][1] = m21;
            m_mat[2][2] = m22;
            m_mat[2][3] = m23;
            m_mat[3][0] = m30;
            m_mat[3][1] = m31;
            m_mat[3][2] = m32;
            m_mat[3][3] = m33;
        }

        Matrix4x4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
        {
            m_mat[0][0] = row0.x;
            m_mat[0][1] = row0.y;
            m_mat[0][2] = row0.z;
            m_mat[0][3] = row0.w;
            m_mat[1][0] = row1.x;
            m_mat[1][1] = row1.y;
            m_mat[1][2] = row1.z;
            m_mat[1][3] = row1.w;
            m_mat[2][0] = row2.x;
            m_mat[2][1] = row2.y;
            m_mat[2][2] = row2.z;
            m_mat[2][3] = row2.w;
            m_mat[3][0] = row3.x;
            m_mat[3][1] = row3.y;
            m_mat[3][2] = row3.z;
            m_mat[3][3] = row3.w;
        }

        static const Matrix4x4 IDENTITY;

        float* operator[](size_t row_index);

        const float* operator[](size_t row_index) const;

        /** Matrix concatenation using '*'.
         */
        Matrix4x4 operator*(const Matrix4x4& m2) const {

            Matrix4x4 r;
            r.m_mat[0][0] = m_mat[0][0] * m2.m_mat[0][0] + m_mat[0][1] * m2.m_mat[1][0] + m_mat[0][2] * m2.m_mat[2][0] +
                m_mat[0][3] * m2.m_mat[3][0];
            r.m_mat[0][1] = m_mat[0][0] * m2.m_mat[0][1] + m_mat[0][1] * m2.m_mat[1][1] + m_mat[0][2] * m2.m_mat[2][1] +
                m_mat[0][3] * m2.m_mat[3][1];
            r.m_mat[0][2] = m_mat[0][0] * m2.m_mat[0][2] + m_mat[0][1] * m2.m_mat[1][2] + m_mat[0][2] * m2.m_mat[2][2] +
                m_mat[0][3] * m2.m_mat[3][2];
            r.m_mat[0][3] = m_mat[0][0] * m2.m_mat[0][3] + m_mat[0][1] * m2.m_mat[1][3] + m_mat[0][2] * m2.m_mat[2][3] +
                m_mat[0][3] * m2.m_mat[3][3];

            r.m_mat[1][0] = m_mat[1][0] * m2.m_mat[0][0] + m_mat[1][1] * m2.m_mat[1][0] + m_mat[1][2] * m2.m_mat[2][0] +
                m_mat[1][3] * m2.m_mat[3][0];
            r.m_mat[1][1] = m_mat[1][0] * m2.m_mat[0][1] + m_mat[1][1] * m2.m_mat[1][1] + m_mat[1][2] * m2.m_mat[2][1] +
                m_mat[1][3] * m2.m_mat[3][1];
            r.m_mat[1][2] = m_mat[1][0] * m2.m_mat[0][2] + m_mat[1][1] * m2.m_mat[1][2] + m_mat[1][2] * m2.m_mat[2][2] +
                m_mat[1][3] * m2.m_mat[3][2];
            r.m_mat[1][3] = m_mat[1][0] * m2.m_mat[0][3] + m_mat[1][1] * m2.m_mat[1][3] + m_mat[1][2] * m2.m_mat[2][3] +
                m_mat[1][3] * m2.m_mat[3][3];

            r.m_mat[2][0] = m_mat[2][0] * m2.m_mat[0][0] + m_mat[2][1] * m2.m_mat[1][0] + m_mat[2][2] * m2.m_mat[2][0] +
                m_mat[2][3] * m2.m_mat[3][0];
            r.m_mat[2][1] = m_mat[2][0] * m2.m_mat[0][1] + m_mat[2][1] * m2.m_mat[1][1] + m_mat[2][2] * m2.m_mat[2][1] +
                m_mat[2][3] * m2.m_mat[3][1];
            r.m_mat[2][2] = m_mat[2][0] * m2.m_mat[0][2] + m_mat[2][1] * m2.m_mat[1][2] + m_mat[2][2] * m2.m_mat[2][2] +
                m_mat[2][3] * m2.m_mat[3][2];
            r.m_mat[2][3] = m_mat[2][0] * m2.m_mat[0][3] + m_mat[2][1] * m2.m_mat[1][3] + m_mat[2][2] * m2.m_mat[2][3] +
                m_mat[2][3] * m2.m_mat[3][3];

            r.m_mat[3][0] = m_mat[3][0] * m2.m_mat[0][0] + m_mat[3][1] * m2.m_mat[1][0] + m_mat[3][2] * m2.m_mat[2][0] +
                m_mat[3][3] * m2.m_mat[3][0];
            r.m_mat[3][1] = m_mat[3][0] * m2.m_mat[0][1] + m_mat[3][1] * m2.m_mat[1][1] + m_mat[3][2] * m2.m_mat[2][1] +
                m_mat[3][3] * m2.m_mat[3][1];
            r.m_mat[3][2] = m_mat[3][0] * m2.m_mat[0][2] + m_mat[3][1] * m2.m_mat[1][2] + m_mat[3][2] * m2.m_mat[2][2] +
                m_mat[3][3] * m2.m_mat[3][2];
            r.m_mat[3][3] = m_mat[3][0] * m2.m_mat[0][3] + m_mat[3][1] * m2.m_mat[1][3] + m_mat[3][2] * m2.m_mat[2][3] +
                m_mat[3][3] * m2.m_mat[3][3];

            return r;
        }

        /** Vector transformation using '*'.
        @remarks
        Transforms the given 3-D vector by the matrix, projecting the
        result back into <i>w</i> = 1.
        @note
        This means that the initial <i>w</i> is considered to be 1.0,
        and then all the three elements of the resulting 3-D vector are
        divided by the resulting <i>w</i>.
        */
        Vector4 operator*(const Vector4& v) const
        {
            return Vector4(m_mat[0][0] * v.x + m_mat[0][1] * v.y + m_mat[0][2] * v.z + m_mat[0][3] * v.w,
                           m_mat[1][0] * v.x + m_mat[1][1] * v.y + m_mat[1][2] * v.z + m_mat[1][3] * v.w,
                           m_mat[2][0] * v.x + m_mat[2][1] * v.y + m_mat[2][2] * v.z + m_mat[2][3] * v.w,
                           m_mat[3][0] * v.x + m_mat[3][1] * v.y + m_mat[3][2] * v.z + m_mat[3][3] * v.w);
        }

        /** Matrix addition.
         */
        Matrix4x4 operator+(const Matrix4x4& m2) const
        {
            Matrix4x4 r;

            r.m_mat[0][0] = m_mat[0][0] + m2.m_mat[0][0];
            r.m_mat[0][1] = m_mat[0][1] + m2.m_mat[0][1];
            r.m_mat[0][2] = m_mat[0][2] + m2.m_mat[0][2];
            r.m_mat[0][3] = m_mat[0][3] + m2.m_mat[0][3];

            r.m_mat[1][0] = m_mat[1][0] + m2.m_mat[1][0];
            r.m_mat[1][1] = m_mat[1][1] + m2.m_mat[1][1];
            r.m_mat[1][2] = m_mat[1][2] + m2.m_mat[1][2];
            r.m_mat[1][3] = m_mat[1][3] + m2.m_mat[1][3];

            r.m_mat[2][0] = m_mat[2][0] + m2.m_mat[2][0];
            r.m_mat[2][1] = m_mat[2][1] + m2.m_mat[2][1];
            r.m_mat[2][2] = m_mat[2][2] + m2.m_mat[2][2];
            r.m_mat[2][3] = m_mat[2][3] + m2.m_mat[2][3];

            r.m_mat[3][0] = m_mat[3][0] + m2.m_mat[3][0];
            r.m_mat[3][1] = m_mat[3][1] + m2.m_mat[3][1];
            r.m_mat[3][2] = m_mat[3][2] + m2.m_mat[3][2];
            r.m_mat[3][3] = m_mat[3][3] + m2.m_mat[3][3];

            return r;
        }

        /** Matrix subtraction.
         */
        Matrix4x4 operator-(const Matrix4x4& m2) const
        {
            Matrix4x4 r;
            r.m_mat[0][0] = m_mat[0][0] - m2.m_mat[0][0];
            r.m_mat[0][1] = m_mat[0][1] - m2.m_mat[0][1];
            r.m_mat[0][2] = m_mat[0][2] - m2.m_mat[0][2];
            r.m_mat[0][3] = m_mat[0][3] - m2.m_mat[0][3];

            r.m_mat[1][0] = m_mat[1][0] - m2.m_mat[1][0];
            r.m_mat[1][1] = m_mat[1][1] - m2.m_mat[1][1];
            r.m_mat[1][2] = m_mat[1][2] - m2.m_mat[1][2];
            r.m_mat[1][3] = m_mat[1][3] - m2.m_mat[1][3];

            r.m_mat[2][0] = m_mat[2][0] - m2.m_mat[2][0];
            r.m_mat[2][1] = m_mat[2][1] - m2.m_mat[2][1];
            r.m_mat[2][2] = m_mat[2][2] - m2.m_mat[2][2];
            r.m_mat[2][3] = m_mat[2][3] - m2.m_mat[2][3];

            r.m_mat[3][0] = m_mat[3][0] - m2.m_mat[3][0];
            r.m_mat[3][1] = m_mat[3][1] - m2.m_mat[3][1];
            r.m_mat[3][2] = m_mat[3][2] - m2.m_mat[3][2];
            r.m_mat[3][3] = m_mat[3][3] - m2.m_mat[3][3];

            return r;
        }

        Matrix4x4 operator*(float scalar) const
        {
            return Matrix4x4(scalar * m_mat[0][0],
                             scalar * m_mat[0][1],
                             scalar * m_mat[0][2],
                             scalar * m_mat[0][3],
                             scalar * m_mat[1][0],
                             scalar * m_mat[1][1],
                             scalar * m_mat[1][2],
                             scalar * m_mat[1][3],
                             scalar * m_mat[2][0],
                             scalar * m_mat[2][1],
                             scalar * m_mat[2][2],
                             scalar * m_mat[2][3],
                             scalar * m_mat[3][0],
                             scalar * m_mat[3][1],
                             scalar * m_mat[3][2],
                             scalar * m_mat[3][3]);
        }

        /** Tests 2 matrices for equality.
         */
        bool operator==(const Matrix4x4& m2) const
        {
            return !(m_mat[0][0] != m2.m_mat[0][0] || m_mat[0][1] != m2.m_mat[0][1] || m_mat[0][2] != m2.m_mat[0][2] ||
                m_mat[0][3] != m2.m_mat[0][3] || m_mat[1][0] != m2.m_mat[1][0] || m_mat[1][1] != m2.m_mat[1][1] ||
                m_mat[1][2] != m2.m_mat[1][2] || m_mat[1][3] != m2.m_mat[1][3] || m_mat[2][0] != m2.m_mat[2][0] ||
                m_mat[2][1] != m2.m_mat[2][1] || m_mat[2][2] != m2.m_mat[2][2] || m_mat[2][3] != m2.m_mat[2][3] ||
                m_mat[3][0] != m2.m_mat[3][0] || m_mat[3][1] != m2.m_mat[3][1] || m_mat[3][2] != m2.m_mat[3][2] ||
                m_mat[3][3] != m2.m_mat[3][3]);
        }

        /** Tests 2 matrices for inequality.
         */
        bool operator!=(const Matrix4x4& m2) const
        {
            return m_mat[0][0] != m2.m_mat[0][0] || m_mat[0][1] != m2.m_mat[0][1] || m_mat[0][2] != m2.m_mat[0][2] ||
                m_mat[0][3] != m2.m_mat[0][3] || m_mat[1][0] != m2.m_mat[1][0] || m_mat[1][1] != m2.m_mat[1][1] ||
                m_mat[1][2] != m2.m_mat[1][2] || m_mat[1][3] != m2.m_mat[1][3] || m_mat[2][0] != m2.m_mat[2][0] ||
                m_mat[2][1] != m2.m_mat[2][1] || m_mat[2][2] != m2.m_mat[2][2] || m_mat[2][3] != m2.m_mat[2][3] ||
                m_mat[3][0] != m2.m_mat[3][0] || m_mat[3][1] != m2.m_mat[3][1] || m_mat[3][2] != m2.m_mat[3][2] ||
                m_mat[3][3] != m2.m_mat[3][3];
        }
        /** Creates a standard 4x4 transformation matrix with a zero translation part from a rotation/scaling 3x3
 * matrix.
 */
        void SetMatrix3x3(const Matrix3x3& mat3)
        {
            m_mat[0][0] = mat3.m_mat[0][0];
            m_mat[0][1] = mat3.m_mat[0][1];
            m_mat[0][2] = mat3.m_mat[0][2];
            m_mat[0][3] = 0;
            m_mat[1][0] = mat3.m_mat[1][0];
            m_mat[1][1] = mat3.m_mat[1][1];
            m_mat[1][2] = mat3.m_mat[1][2];
            m_mat[1][3] = 0;
            m_mat[2][0] = mat3.m_mat[2][0];
            m_mat[2][1] = mat3.m_mat[2][1];
            m_mat[2][2] = mat3.m_mat[2][2];
            m_mat[2][3] = 0;
            m_mat[3][0] = 0;
            m_mat[3][1] = 0;
            m_mat[3][2] = 0;
            m_mat[3][3] = 1;
        }
        Matrix4x4 Transpose() const
        {
            return Matrix4x4(m_mat[0][0],
                             m_mat[1][0],
                             m_mat[2][0],
                             m_mat[3][0],
                             m_mat[0][1],
                             m_mat[1][1],
                             m_mat[2][1],
                             m_mat[3][1],
                             m_mat[0][2],
                             m_mat[1][2],
                             m_mat[2][2],
                             m_mat[3][2],
                             m_mat[0][3],
                             m_mat[1][3],
                             m_mat[2][3],
                             m_mat[3][3]);
        }

        //-----------------------------------------------------------------------

        /*
        -----------------------------------------------------------------------
        Translation Transformation
        -----------------------------------------------------------------------
        */
        /** Sets the translation transformation part of the matrix.
         */
        void GetTrans(const Vector3& v)
        {
            m_mat[0][3] = v.x;
            m_mat[1][3] = v.y;
            m_mat[2][3] = v.z;
        }

        /** Extracts the translation transformation part of the matrix.
         */
        Vector3 SetTrans() const { return Vector3(m_mat[0][3], m_mat[1][3], m_mat[2][3]); }

        Matrix4x4 buildViewportMatrix(unsigned int width, unsigned int height)
        {
            return Matrix4x4(0.5f * (float)width,
                             0.0f,
                             0.0f,
                             0.5f * (float)width,
                             0.0f,
                             -0.5f * (float)height,
                             0.0f,
                             0.5f * (float)height,
                             0.0f,
                             0.0f,
                             -1.0f,
                             1.0f,
                             0.0f,
                             0.0f,
                             0.0f,
                             1.0f);
        }

        static Matrix4x4 MirrorMatrix(Vector4 mirror_plane)
        {
            Matrix4x4 result;
            result.m_mat[0][0] = -2 * mirror_plane.x * mirror_plane.x + 1;
            result.m_mat[1][0] = -2 * mirror_plane.x * mirror_plane.y;
            result.m_mat[2][0] = -2 * mirror_plane.x * mirror_plane.z;
            result.m_mat[3][0] = 0;

            result.m_mat[0][1] = -2 * mirror_plane.y * mirror_plane.x;
            result.m_mat[1][1] = -2 * mirror_plane.y * mirror_plane.y + 1;
            result.m_mat[2][1] = -2 * mirror_plane.y * mirror_plane.z;
            result.m_mat[3][1] = 0;

            result.m_mat[0][2] = -2 * mirror_plane.z * mirror_plane.x;
            result.m_mat[1][2] = -2 * mirror_plane.z * mirror_plane.y;
            result.m_mat[2][2] = -2 * mirror_plane.z * mirror_plane.z + 1;
            result.m_mat[3][2] = 0;

            result.m_mat[0][3] = -2 * mirror_plane.w * mirror_plane.x;
            result.m_mat[1][3] = -2 * mirror_plane.w * mirror_plane.y;
            result.m_mat[2][3] = -2 * mirror_plane.w * mirror_plane.z;
            result.m_mat[3][3] = 1;

            return result;
        }

        static Matrix4x4 RotationMatrix(Vector3 normal);

        /** Builds a translation matrix
         */
        void makeTrans(const Vector3& v)
        {
            m_mat[0][0] = 1.0;
            m_mat[0][1] = 0.0;
            m_mat[0][2] = 0.0;
            m_mat[0][3] = v.x;
            m_mat[1][0] = 0.0;
            m_mat[1][1] = 1.0;
            m_mat[1][2] = 0.0;
            m_mat[1][3] = v.y;
            m_mat[2][0] = 0.0;
            m_mat[2][1] = 0.0;
            m_mat[2][2] = 1.0;
            m_mat[2][3] = v.z;
            m_mat[3][0] = 0.0;
            m_mat[3][1] = 0.0;
            m_mat[3][2] = 0.0;
            m_mat[3][3] = 1.0;
        }

        void makeTrans(float tx, float ty, float tz)
        {
            m_mat[0][0] = 1.0;
            m_mat[0][1] = 0.0;
            m_mat[0][2] = 0.0;
            m_mat[0][3] = tx;
            m_mat[1][0] = 0.0;
            m_mat[1][1] = 1.0;
            m_mat[1][2] = 0.0;
            m_mat[1][3] = ty;
            m_mat[2][0] = 0.0;
            m_mat[2][1] = 0.0;
            m_mat[2][2] = 1.0;
            m_mat[2][3] = tz;
            m_mat[3][0] = 0.0;
            m_mat[3][1] = 0.0;
            m_mat[3][2] = 0.0;
            m_mat[3][3] = 1.0;
        }

        /*
        -----------------------------------------------------------------------
        Scale Transformation
        -----------------------------------------------------------------------
        */
        /** Sets the scale part of the matrix.
         */
        void SetScale(const Vector3& v)
        {
            m_mat[0][0] = v.x;
            m_mat[1][1] = v.y;
            m_mat[2][2] = v.z;
        }

        /** Gets a scale matrix - variation for not using a vector.
         */
        static Matrix4x4 BuildScaleMatrix(float s_x, float s_y, float s_z)
        {
            Matrix4x4 r;
            r.m_mat[0][0] = s_x;
            r.m_mat[0][1] = 0.0;
            r.m_mat[0][2] = 0.0;
            r.m_mat[0][3] = 0.0;
            r.m_mat[1][0] = 0.0;
            r.m_mat[1][1] = s_y;
            r.m_mat[1][2] = 0.0;
            r.m_mat[1][3] = 0.0;
            r.m_mat[2][0] = 0.0;
            r.m_mat[2][1] = 0.0;
            r.m_mat[2][2] = s_z;
            r.m_mat[2][3] = 0.0;
            r.m_mat[3][0] = 0.0;
            r.m_mat[3][1] = 0.0;
            r.m_mat[3][2] = 0.0;
            r.m_mat[3][3] = 1.0;

            return r;
        }

        /** Extracts the rotation / scaling part of the Matrix as a 3x3 matrix.
        @param m3x3 Destination Matrix3
        */
        void Extract3x3Matrix(Matrix3x3& m3x3) const
        {
            m3x3.m_mat[0][0] = m_mat[0][0];
            m3x3.m_mat[0][1] = m_mat[0][1];
            m3x3.m_mat[0][2] = m_mat[0][2];
            m3x3.m_mat[1][0] = m_mat[1][0];
            m3x3.m_mat[1][1] = m_mat[1][1];
            m3x3.m_mat[1][2] = m_mat[1][2];
            m3x3.m_mat[2][0] = m_mat[2][0];
            m3x3.m_mat[2][1] = m_mat[2][1];
            m3x3.m_mat[2][2] = m_mat[2][2];
        }

        void ExtractAxes(Vector3& out_x, Vector3& out_y, Vector3& out_z) const
        {
            out_x = Vector3(m_mat[0][0], m_mat[1][0], m_mat[2][0]);
            out_x.Normalize();
            out_y = Vector3(m_mat[0][1], m_mat[1][1], m_mat[2][1]);
            out_y.Normalize();
            out_z = Vector3(m_mat[0][2], m_mat[1][2], m_mat[2][2]);
            out_z.Normalize();
        }

        /** Building a Matrix4 from orientation / scale / position.
        @remarks
        Transform is performed in the order scale, rotate, translation, i.e. translation is independent
        of orientation axes, scale does not affect size of translation, rotation and scaling are always
        centered on the origin.
        */
        void MakeTransform(const Vector3& position, const Vector3& scale, const Quaternion& rotation) {
            // quaternion to rotate matrix
            float fTx = rotation.x + rotation.x;   // 2x
            float fTy = rotation.y + rotation.y;   // 2y
            float fTz = rotation.z + rotation.z;   // 2z
            float fTwx = fTx * rotation.w; // 2xw
            float fTwy = fTy * rotation.w; // 2yw
            float fTwz = fTz * rotation.w; // 2z2
            float fTxx = fTx * rotation.x; // 2x^2
            float fTxy = fTy * rotation.x; // 2xy
            float fTxz = fTz * rotation.x; // 2xz
            float fTyy = fTy * rotation.y; // 2y^2
            float fTyz = fTz * rotation.y; // 2yz
            float fTzz = fTz * rotation.z; // 2z^2

            m_mat[0][0] = 1.0f - (fTyy + fTzz); // 1 - 2y^2 - 2z^2
            m_mat[0][1] = fTxy - fTwz;          // 2xy - 2wz
            m_mat[0][2] = fTxz + fTwy;          // 2xz + 2wy
            m_mat[1][0] = fTxy + fTwz;          // 2xy + 2wz
            m_mat[1][1] = 1.0f - (fTxx + fTzz); // 1 - 2x^2 - 2z^2
            m_mat[1][2] = fTyz - fTwx;          // 2yz - 2wx
            m_mat[2][0] = fTxz - fTwy;          // 2xz - 2wy
            m_mat[2][1] = fTyz + fTwx;          // 2yz + 2wx
            m_mat[2][2] = 1.0f - (fTxx + fTyy); // 1 - 2x^2 - 2y^2

            // Set up final matrix with scale, rotation and translation
            m_mat[0][0] = scale.x * m_mat[0][0];
            m_mat[0][1] = scale.y * m_mat[0][1];
            m_mat[0][2] = scale.z * m_mat[0][2];
            m_mat[0][3] = position.x;
            m_mat[1][0] = scale.x * m_mat[1][0];
            m_mat[1][1] = scale.y * m_mat[1][1];
            m_mat[1][2] = scale.z * m_mat[1][2];
            m_mat[1][3] = position.y;
            m_mat[2][0] = scale.x * m_mat[2][0];
            m_mat[2][1] = scale.y * m_mat[2][1];
            m_mat[2][2] = scale.z * m_mat[2][2];
            m_mat[2][3] = position.z;

            // No projection term
            m_mat[3][0] = 0;
            m_mat[3][1] = 0;
            m_mat[3][2] = 0;
            m_mat[3][3] = 1;
        };


        /** Building an inverse Matrix4 from orientation / scale / position.
        @remarks
        As MakeTransform except it build the inverse given the same data as MakeTransform, so
        performing -translation, -rotate, 1/scale in that order.
        */
        void MakeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& rotation) {
            // Invert the parameters
            Vector3    inv_translate = -position;
            Vector3    inv_scale(1 / scale.x, 1 / scale.y, 1 / scale.z);
            Quaternion inv_rot = rotation.Inverse();

            // Because we're inverting, order is translation, rotation, scale
            // So make translation relative to scale & rotation
            inv_translate = inv_rot * inv_translate; // rotate
            inv_translate *= inv_scale;              // scale

            // Next, make a 3x3 rotation matrix
            Matrix3x3 rot3x3;
            QuaternionToMatrix(rotation, rot3x3);

            // Set up final matrix with scale, rotation and translation
            m_mat[0][0] = inv_scale.x * rot3x3[0][0];
            m_mat[0][1] = inv_scale.x * rot3x3[0][1];
            m_mat[0][2] = inv_scale.x * rot3x3[0][2];
            m_mat[0][3] = inv_translate.x;
            m_mat[1][0] = inv_scale.y * rot3x3[1][0];
            m_mat[1][1] = inv_scale.y * rot3x3[1][1];
            m_mat[1][2] = inv_scale.y * rot3x3[1][2];
            m_mat[1][3] = inv_translate.y;
            m_mat[2][0] = inv_scale.z * rot3x3[2][0];
            m_mat[2][1] = inv_scale.z * rot3x3[2][1];
            m_mat[2][2] = inv_scale.z * rot3x3[2][2];
            m_mat[2][3] = inv_translate.z;

            // No projection term
            m_mat[3][0] = 0;
            m_mat[3][1] = 0;
            m_mat[3][2] = 0;
            m_mat[3][3] = 1;
        };

        /** Decompose a Matrix4 to orientation / scale / position.
         */
        void Decomposition(Vector3& position, Vector3& scale, Quaternion& rotate) const;

        Matrix4x4 Inverse() const
        {
            float m00 = m_mat[0][0], m01 = m_mat[0][1], m02 = m_mat[0][2], m03 = m_mat[0][3];
            float m10 = m_mat[1][0], m11 = m_mat[1][1], m12 = m_mat[1][2], m13 = m_mat[1][3];
            float m20 = m_mat[2][0], m21 = m_mat[2][1], m22 = m_mat[2][2], m23 = m_mat[2][3];
            float m30 = m_mat[3][0], m31 = m_mat[3][1], m32 = m_mat[3][2], m33 = m_mat[3][3];

            float v0 = m20 * m31 - m21 * m30;
            float v1 = m20 * m32 - m22 * m30;
            float v2 = m20 * m33 - m23 * m30;
            float v3 = m21 * m32 - m22 * m31;
            float v4 = m21 * m33 - m23 * m31;
            float v5 = m22 * m33 - m23 * m32;

            float t00 = +(v5 * m11 - v4 * m12 + v3 * m13);
            float t10 = -(v5 * m10 - v2 * m12 + v1 * m13);
            float t20 = +(v4 * m10 - v2 * m11 + v0 * m13);
            float t30 = -(v3 * m10 - v1 * m11 + v0 * m12);

            float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

            float d00 = t00 * invDet;
            float d10 = t10 * invDet;
            float d20 = t20 * invDet;
            float d30 = t30 * invDet;

            float d01 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            float d11 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            float d21 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            float d31 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            v0 = m10 * m31 - m11 * m30;
            v1 = m10 * m32 - m12 * m30;
            v2 = m10 * m33 - m13 * m30;
            v3 = m11 * m32 - m12 * m31;
            v4 = m11 * m33 - m13 * m31;
            v5 = m12 * m33 - m13 * m32;

            float d02 = +(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            float d12 = -(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            float d22 = +(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            float d32 = -(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            v0 = m21 * m10 - m20 * m11;
            v1 = m22 * m10 - m20 * m12;
            v2 = m23 * m10 - m20 * m13;
            v3 = m22 * m11 - m21 * m12;
            v4 = m23 * m11 - m21 * m13;
            v5 = m23 * m12 - m22 * m13;

            float d03 = -(v5 * m01 - v4 * m02 + v3 * m03) * invDet;
            float d13 = +(v5 * m00 - v2 * m02 + v1 * m03) * invDet;
            float d23 = -(v4 * m00 - v2 * m01 + v0 * m03) * invDet;
            float d33 = +(v3 * m00 - v1 * m01 + v0 * m02) * invDet;

            return Matrix4x4(d00, d01, d02, d03, d10, d11, d12, d13, d20, d21, d22, d23, d30, d31, d32, d33);
        }

        Vector3 TransformCoord(const Vector3& v)
        {
            Vector4 temp(v, 1.0f);
            Vector4 ret = (*this) * temp;
            if (ret.w == 0.0f)
            {
                return Vector3{};
            }
            else
            {
                ret /= ret.w;
                return Vector3(ret.x, ret.y, ret.z);
            }

            return Vector3{};
        }
    };
}