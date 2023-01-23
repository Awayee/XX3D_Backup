#include "Matrix.h"
#include "Core/macro.h"
#include <cmath>

namespace Math {
    // matrix 3x3
    MATH_GENERIC Matrix3x3<T>::Matrix3x3(T** arr)
	{
		memcpy(m_mat[0], arr[0], 3 * sizeof(T));
		memcpy(m_mat[1], arr[1], 3 * sizeof(T));
		memcpy(m_mat[2], arr[2], 3 * sizeof(T));
	}
    MATH_GENERIC Matrix3x3<T>::Matrix3x3(T* arr)
    {
        m_mat[0][0] = arr[0]; m_mat[0][1] = arr[1]; m_mat[0][2] = arr[2];
        m_mat[1][0] = arr[3]; m_mat[1][1] = arr[4]; m_mat[1][2] = arr[5];
        m_mat[2][0] = arr[6]; m_mat[2][1] = arr[7]; m_mat[2][2] = arr[8];
    }
    MATH_GENERIC Matrix3x3<T>::Matrix3x3(T entry00, T entry01, T entry02, T entry10, T entry11, T entry12, T entry20, T entry21, T entry22)
    {
        m_mat[0][0] = entry00; m_mat[0][1] = entry01; m_mat[0][2] = entry02;
        m_mat[1][0] = entry10; m_mat[1][1] = entry11;  m_mat[1][2] = entry12;
        m_mat[2][0] = entry20; m_mat[2][1] = entry21; m_mat[2][2] = entry22;
    }
    MATH_GENERIC Matrix3x3<T>::Matrix3x3(const Vector3<T>& row0, const Vector3<T>& row1, const Vector3<T>& row2)
    {
        m_mat[0][0] = row0.x; m_mat[0][1] = row0.y; m_mat[0][2] = row0.z;
        m_mat[1][0] = row1.x; m_mat[1][1] = row1.y; m_mat[1][2] = row1.z;
        m_mat[2][0] = row2.x; m_mat[2][1] = row2.y; m_mat[2][2] = row2.z;
    }
    MATH_GENERIC bool Matrix3x3<T>::operator==(const Matrix3x3<T>& rhs) const
    {
        for (int row_index = 0; row_index < 3; row_index++)
        {
            for (int col_index = 0; col_index < 3; col_index++)
            {
                if (m_mat[row_index][col_index] != rhs.m_mat[row_index][col_index])
                    return false;
            }
        }

        return true;
    }
    MATH_GENERIC Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& rhs) const
    {
        Matrix3x3<T> sum;
        for (int row_index = 0; row_index < 3; row_index++)
        {
            for (int col_index = 0; col_index < 3; col_index++)
            {
                sum.m_mat[row_index][col_index] = m_mat[row_index][col_index] + rhs.m_mat[row_index][col_index];
            }
        }
        return sum;
    }
    MATH_GENERIC Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& rhs) const
    {
        Matrix3x3<T> diff;
        for (int row_index = 0; row_index < 3; row_index++)
        {
            for (int col_index = 0; col_index < 3; col_index++)
            {
                diff.m_mat[row_index][col_index] = m_mat[row_index][col_index] - rhs.m_mat[row_index][col_index];
            }
        }
        return diff;
    }
    MATH_GENERIC Matrix3x3<T> Matrix3x3<T>::operator-() const
    {
        return Matrix3x3<T>{
            -m_mat[0][0], -m_mat[0][1], -m_mat[0][2],
            -m_mat[1][0], -m_mat[1][1], -m_mat[1][2],
            -m_mat[2][0], -m_mat[2][1], -m_mat[2][2]
        };
    }
    MATH_GENERIC Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& rhs) const
    {
        Matrix3x3<T> prod;
        for (int rowIdx = 0; rowIdx < 3; rowIdx++)
        {
            for (int colIdx = 0; colIdx < 3; colIdx++)
            {
                prod.m_mat[rowIdx][colIdx] = m_mat[rowIdx][0] * rhs.m_mat[0][colIdx] +
                    m_mat[rowIdx][1] * rhs.m_mat[1][colIdx] +
                    m_mat[rowIdx][2] * rhs.m_mat[2][colIdx];
            }
        }
        return prod;
    }
    MATH_GENERIC Vector3<T> Matrix3x3<T>::operator*(const Vector3<T>& rhs) const
    {
        Vector3<float> prod;
        for (int rowIdx = 0; rowIdx < 3; rowIdx++)
        {
            prod[rowIdx] =
                m_mat[rowIdx][0] * rhs.x + m_mat[rowIdx][1] * rhs.y + m_mat[rowIdx][2] * rhs.z;
        }
        return prod;
    }
    MATH_GENERIC Matrix3x3<T> Matrix3x3<T>::operator*(T scalar) const
    {
        Matrix3x3<T> prod;
        for (int row_index = 0; row_index < 3; row_index++)
        {
            for (int col_index = 0; col_index < 3; col_index++)
                prod[row_index][col_index] = scalar * m_mat[row_index][col_index];
        }
        return prod;
    }
    MATH_GENERIC Matrix3x3<T> Matrix3x3<T>::Transpose() const
    {
        Matrix3x3<T> transpose_v;
        for (int row_index = 0; row_index < 3; row_index++)
        {
            for (int col_index = 0; col_index < 3; col_index++)
                transpose_v[row_index][col_index] = m_mat[col_index][row_index];
        }
        return transpose_v;
    }
    MATH_GENERIC float Matrix3x3<T>::Determinant() const
    {
        float cofactor00 = m_mat[1][1] * m_mat[2][2] - m_mat[1][2] * m_mat[2][1];
        float cofactor10 = m_mat[1][2] * m_mat[2][0] - m_mat[1][0] * m_mat[2][2];
        float cofactor20 = m_mat[1][0] * m_mat[2][1] - m_mat[1][1] * m_mat[2][0];

        float det = m_mat[0][0] * cofactor00 + m_mat[0][1] * cofactor10 + m_mat[0][2] * cofactor20;

        return det;
    }
    MATH_GENERIC  bool Matrix3x3<T>::Inverse(Matrix3x3<T>& inv_mat, float fTolerance) const
    {
        // Invert a 3x3 using cofactors.  This is about 8 times faster than
        // the Numerical Recipes code which uses Gaussian elimination.

        float det = Determinant();
        if (std::fabs(det) <= fTolerance)
            return false;

        inv_mat[0][0] = m_mat[1][1] * m_mat[2][2] - m_mat[1][2] * m_mat[2][1];
        inv_mat[0][1] = m_mat[0][2] * m_mat[2][1] - m_mat[0][1] * m_mat[2][2];
        inv_mat[0][2] = m_mat[0][1] * m_mat[1][2] - m_mat[0][2] * m_mat[1][1];
        inv_mat[1][0] = m_mat[1][2] * m_mat[2][0] - m_mat[1][0] * m_mat[2][2];
        inv_mat[1][1] = m_mat[0][0] * m_mat[2][2] - m_mat[0][2] * m_mat[2][0];
        inv_mat[1][2] = m_mat[0][2] * m_mat[1][0] - m_mat[0][0] * m_mat[1][2];
        inv_mat[2][0] = m_mat[1][0] * m_mat[2][1] - m_mat[1][1] * m_mat[2][0];
        inv_mat[2][1] = m_mat[0][1] * m_mat[2][0] - m_mat[0][0] * m_mat[2][1];
        inv_mat[2][2] = m_mat[0][0] * m_mat[1][1] - m_mat[0][1] * m_mat[1][0];

        float inv_det = 1.0f / det;
        for (int row_index = 0; row_index < 3; row_index++)
        {
            for (int col_index = 0; col_index < 3; col_index++)
                inv_mat[row_index][col_index] *= inv_det;
        }

        return true;
    }

    MATH_GENERIC Matrix3x3<T> Matrix3x3<T>::Inverse(T tolerance)
    {
        Matrix3x3<T> inv;
        Inverse(inv, tolerance);
        return inv;
    }

    MATH_GENERIC Matrix3x3<T> operator*(T scalar, const Matrix3x3<T>& rhs)
    {
        Matrix3x3 prod;
        for (int row_index = 0; row_index < 3; row_index++)
        {
            for (int col_index = 0; col_index < 3; col_index++)
                prod[row_index][col_index] = scalar * rhs.m_mat[row_index][col_index];
        }
        return prod;
    }

    MATH_GENERIC void MatrixToQuaternion(const Matrix3x3<T>& m, Quaternion<T>& q)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".

        float trace = m[0][0] + m[1][1] + m[2][2];
        float root;

        if (trace > 0.0)
        {
            // |w| > 1/2, may as well choose w > 1/2
            root = std::sqrt(trace + 1.0f); // 2w
            q.w = 0.5f * root;
            root = 0.5f / root; // 1/(4w)
            q.x = (m[2][1] - m[1][2]) * root;
            q.y = (m[0][2] - m[2][0]) * root;
            q.z = (m[1][0] - m[0][1]) * root;
        }
        else
        {
            // |w| <= 1/2
            int s_iNext[3] = { 1, 2, 0 };
            int i = 0;
            if (m[1][1] > m[0][0])
                i = 1;
            if (m[2][2] > m[i][i])
                i = 2;
            int j = s_iNext[i];
            int k = s_iNext[j];

            root = std::sqrt(m[i][i] - m[j][j] - m[k][k] + 1.0f);
            float* apkQuat[3] = { &q.x, &q.y, &q.z };
            *apkQuat[i] = 0.5f * root;
            root = 0.5f / root;
            q.w = (m[k][j] - m[j][k]) * root;
            *apkQuat[j] = (m[j][i] + m[i][j]) * root;
            *apkQuat[k] = (m[k][i] + m[i][k]) * root;
        }
    }

    MATH_GENERIC void QuaternionToMatrix(const Quaternion<T>& q, Matrix3x3<T>& m) {
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

    MATH_GENERIC const Matrix3x3<T> Matrix3x3<T>::IDENTITY = Matrix3x3<T>(1, 1, 1, 1, 1, 1, 1, 1, 1);


    // matrix 4x4
    MATH_GENERIC const Matrix4x4<T> Matrix4x4<T>::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    MATH_GENERIC const Matrix4x4<T> Matrix4x4<T>::ZERO(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    MATH_GENERIC Matrix4x4<T>::Matrix4x4(const Matrix4x4<T>& mat)
    {
        m_mat[0][0] = mat[0][0]; m_mat[0][1] = mat[0][1]; m_mat[0][2] = mat[0][2]; m_mat[0][3] = mat[0][3];
        m_mat[1][0] = mat[1][0]; m_mat[1][1] = mat[1][1]; m_mat[1][2] = mat[1][2]; m_mat[1][3] = mat[1][3];
        m_mat[2][0] = mat[2][0]; m_mat[2][1] = mat[2][1]; m_mat[2][2] = mat[2][2]; m_mat[2][3] = mat[2][3];
        m_mat[3][0] = mat[3][0]; m_mat[3][1] = mat[3][1]; m_mat[3][2] = mat[3][2]; m_mat[3][3] = mat[3][3];
    }
    MATH_GENERIC Matrix4x4<T>::Matrix4x4(const T* matrixArray)
    {
        m_mat[0][0] = matrixArray[0];  m_mat[0][1] = matrixArray[1];  m_mat[0][2] = matrixArray[2];  m_mat[0][3] = matrixArray[3];
        m_mat[1][0] = matrixArray[4];  m_mat[1][1] = matrixArray[5];  m_mat[1][2] = matrixArray[6];  m_mat[1][3] = matrixArray[7];
        m_mat[2][0] = matrixArray[8];  m_mat[2][1] = matrixArray[9];  m_mat[2][2] = matrixArray[10]; m_mat[2][3] = matrixArray[11];
        m_mat[3][0] = matrixArray[12]; m_mat[3][1] = matrixArray[13]; m_mat[3][2] = matrixArray[14]; m_mat[3][3] = matrixArray[15];
    }
    MATH_GENERIC Matrix4x4<T>::Matrix4x4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13, T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33)
    {
        m_mat[0][0] = m00; m_mat[0][1] = m01; m_mat[0][2] = m02; m_mat[0][3] = m03;
        m_mat[1][0] = m10; m_mat[1][1] = m11; m_mat[1][2] = m12; m_mat[1][3] = m13;
        m_mat[2][0] = m20; m_mat[2][1] = m21; m_mat[2][2] = m22; m_mat[2][3] = m23;
        m_mat[3][0] = m30; m_mat[3][1] = m31; m_mat[3][2] = m32; m_mat[3][3] = m33;
    }
    MATH_GENERIC Matrix4x4<T>::Matrix4x4(const Vector4<T>& row0, const Vector4<T>& row1, const Vector4<T>& row2, const Vector4<T>& row3)
    {
        m_mat[0][0] = row0.x; m_mat[0][1] = row0.y; m_mat[0][2] = row0.z; m_mat[0][3] = row0.w;
        m_mat[1][0] = row1.x; m_mat[1][1] = row1.y; m_mat[1][2] = row1.z; m_mat[1][3] = row1.w;
        m_mat[2][0] = row2.x; m_mat[2][1] = row2.y; m_mat[2][2] = row2.z; m_mat[2][3] = row2.w;
        m_mat[3][0] = row3.x; m_mat[3][1] = row3.y; m_mat[3][2] = row3.z; m_mat[3][3] = row3.w;
    }
    MATH_GENERIC T* Matrix4x4<T>::operator[](int row_index)
	{
		ASSERT(row_index < 4);
		return m_mat[row_index];
	}
    MATH_GENERIC const T* Matrix4x4<T>::operator[](int row_index) const
	{
		ASSERT(row_index < 4);
		return m_mat[row_index];
	}
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& m2) const
    {
        Matrix4x4<T> r;
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
    MATH_GENERIC Vector4<T> Matrix4x4<T>::operator*(const Vector4<T>& v) const
    {
        return Vector4<float>(m_mat[0][0] * v.x + m_mat[0][1] * v.y + m_mat[0][2] * v.z + m_mat[0][3] * v.w,
            m_mat[1][0] * v.x + m_mat[1][1] * v.y + m_mat[1][2] * v.z + m_mat[1][3] * v.w,
            m_mat[2][0] * v.x + m_mat[2][1] * v.y + m_mat[2][2] * v.z + m_mat[2][3] * v.w,
            m_mat[3][0] * v.x + m_mat[3][1] * v.y + m_mat[3][2] * v.z + m_mat[3][3] * v.w);
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& m2) const
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
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& m2) const
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
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::operator*(float scalar) const
    {
        return Matrix4x4<T>(scalar * m_mat[0][0], scalar * m_mat[0][1], scalar * m_mat[0][2], scalar * m_mat[0][3],
            scalar * m_mat[1][0], scalar * m_mat[1][1], scalar * m_mat[1][2], scalar * m_mat[1][3],
            scalar * m_mat[2][0], scalar * m_mat[2][1], scalar * m_mat[2][2], scalar * m_mat[2][3],
            scalar * m_mat[3][0], scalar * m_mat[3][1], scalar * m_mat[3][2], scalar * m_mat[3][3]);
    }
    MATH_GENERIC bool Matrix4x4<T>::operator==(const Matrix4x4<T>& m2) const
    {
        return !(m_mat[0][0] != m2.m_mat[0][0] || m_mat[0][1] != m2.m_mat[0][1] || m_mat[0][2] != m2.m_mat[0][2] ||
            m_mat[0][3] != m2.m_mat[0][3] || m_mat[1][0] != m2.m_mat[1][0] || m_mat[1][1] != m2.m_mat[1][1] ||
            m_mat[1][2] != m2.m_mat[1][2] || m_mat[1][3] != m2.m_mat[1][3] || m_mat[2][0] != m2.m_mat[2][0] ||
            m_mat[2][1] != m2.m_mat[2][1] || m_mat[2][2] != m2.m_mat[2][2] || m_mat[2][3] != m2.m_mat[2][3] ||
            m_mat[3][0] != m2.m_mat[3][0] || m_mat[3][1] != m2.m_mat[3][1] || m_mat[3][2] != m2.m_mat[3][2] ||
            m_mat[3][3] != m2.m_mat[3][3]);
    }
    MATH_GENERIC bool Matrix4x4<T>::operator!=(const Matrix4x4<T>& m2) const
    {
        return m_mat[0][0] != m2.m_mat[0][0] || m_mat[0][1] != m2.m_mat[0][1] || m_mat[0][2] != m2.m_mat[0][2] ||
            m_mat[0][3] != m2.m_mat[0][3] || m_mat[1][0] != m2.m_mat[1][0] || m_mat[1][1] != m2.m_mat[1][1] ||
            m_mat[1][2] != m2.m_mat[1][2] || m_mat[1][3] != m2.m_mat[1][3] || m_mat[2][0] != m2.m_mat[2][0] ||
            m_mat[2][1] != m2.m_mat[2][1] || m_mat[2][2] != m2.m_mat[2][2] || m_mat[2][3] != m2.m_mat[2][3] ||
            m_mat[3][0] != m2.m_mat[3][0] || m_mat[3][1] != m2.m_mat[3][1] || m_mat[3][2] != m2.m_mat[3][2] ||
            m_mat[3][3] != m2.m_mat[3][3];
    }
    MATH_GENERIC void Matrix4x4<T>::SetMatrix3x3(const Matrix3x3<T>& mat3)
    {
        m_mat[0][0] = mat3.m_mat[0][0]; m_mat[0][1] = mat3.m_mat[0][1]; m_mat[0][2] = mat3.m_mat[0][2]; m_mat[0][3] = 0;
        m_mat[1][0] = mat3.m_mat[1][0]; m_mat[1][1] = mat3.m_mat[1][1]; m_mat[1][2] = mat3.m_mat[1][2]; m_mat[1][3] = 0;
        m_mat[2][0] = mat3.m_mat[2][0]; m_mat[2][1] = mat3.m_mat[2][1]; m_mat[2][2] = mat3.m_mat[2][2]; m_mat[2][3] = 0;
        m_mat[3][0] = 0;                m_mat[3][1] = 0;                m_mat[3][2] = 0;                m_mat[3][3] = 1;
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::Transpose() const
    {
        return Matrix4x4<T>(m_mat[0][0], m_mat[1][0], m_mat[2][0], m_mat[3][0],
            m_mat[0][1], m_mat[1][1], m_mat[2][1], m_mat[3][1],
            m_mat[0][2], m_mat[1][2], m_mat[2][2], m_mat[3][2],
            m_mat[0][3], m_mat[1][3], m_mat[2][3], m_mat[3][3]);
    }
    MATH_GENERIC void Matrix4x4<T>::SetTranslate(const Vector3<T>& v)
    {
        m_mat[0][3] = v.x;
        m_mat[1][3] = v.y;
        m_mat[2][3] = v.z;
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::BuildViewportMatrix(unsigned int width, unsigned int height)
    {
        return Matrix4x4(0.5f * (float)width, 0.0f, 0.0f, 0.5f * (float)width,
            0.0f, -0.5f * (float)height, 0.0f, 0.5f * (float)height,
            0.0f, 0.0f, -1.0f, 1.0f,
            0.0f, 0.0f, 0.0f, 1.0f);
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::MirrorMatrix(Vector4<T> mirror_plane)
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
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::RotationMatrix(Vector3<T> normal)
	{
		Vector3<float> up = Vector3<float>(0, 0, 1);
		if (fabs(normal.z) > 0.999f)
		{
			up = Vector3<float>(0, 1, 0);
		}

		Vector3<float> left = Vector3<float>::Cross(up, normal);
		up = Vector3<float>::Cross(normal, left);

		left.Normalize();
		up.Normalize();

		Matrix4x4 result = Matrix4x4::IDENTITY;
		result.SetMatrix3x3(Matrix3x3(left, up, normal));

		return result.Transpose();
	}
    MATH_GENERIC void Matrix4x4<T>::MakeTrans(const Vector3<T>& v)
    {
        m_mat[0][0] = 1.0; m_mat[0][1] = 0.0; m_mat[0][2] = 0.0; m_mat[0][3] = v.x;
        m_mat[1][0] = 0.0; m_mat[1][1] = 1.0; m_mat[1][2] = 0.0; m_mat[1][3] = v.y;
        m_mat[2][0] = 0.0; m_mat[2][1] = 0.0; m_mat[2][2] = 1.0; m_mat[2][3] = v.z;
        m_mat[3][0] = 0.0; m_mat[3][1] = 0.0; m_mat[3][2] = 0.0; m_mat[3][3] = 1.0;
    }
    MATH_GENERIC void Matrix4x4<T>::MakeTrans(T tx, T ty, T tz)
    {
        m_mat[0][0] = 1.0; m_mat[0][1] = 0.0; m_mat[0][2] = 0.0; m_mat[0][3] = tx;
        m_mat[1][0] = 0.0; m_mat[1][1] = 1.0; m_mat[1][2] = 0.0; m_mat[1][3] = ty;
        m_mat[2][0] = 0.0; m_mat[2][1] = 0.0; m_mat[2][2] = 1.0; m_mat[2][3] = tz;
        m_mat[3][0] = 0.0; m_mat[3][1] = 0.0; m_mat[3][2] = 0.0; m_mat[3][3] = 1.0;
    }
    MATH_GENERIC void Matrix4x4<T>::SetScale(const Vector3<T>& v)
    {
        m_mat[0][0] = v.x;
        m_mat[1][1] = v.y;
        m_mat[2][2] = v.z;
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::BuildScaleMatrix(T s_x, T s_y, T s_z)
    {
        Matrix4x4 r;
        r.m_mat[0][0] = s_x; r.m_mat[0][1] = 0.0; r.m_mat[0][2] = 0.0; r.m_mat[0][3] = 0.0;
        r.m_mat[1][0] = 0.0; r.m_mat[1][1] = s_y; r.m_mat[1][2] = 0.0; r.m_mat[1][3] = 0.0;
        r.m_mat[2][0] = 0.0; r.m_mat[2][1] = 0.0; r.m_mat[2][2] = s_z; r.m_mat[2][3] = 0.0;
        r.m_mat[3][0] = 0.0; r.m_mat[3][1] = 0.0; r.m_mat[3][2] = 0.0; r.m_mat[3][3] = 1.0;
        return r;
    }
    MATH_GENERIC void Matrix4x4<T>::ExtractMatrix3x3(Matrix3x3<T>& m3x3)
    {
        m3x3.m_mat[0][0] = m_mat[0][0]; m3x3.m_mat[0][1] = m_mat[0][1]; m3x3.m_mat[0][2] = m_mat[0][2];
        m3x3.m_mat[1][0] = m_mat[1][0]; m3x3.m_mat[1][1] = m_mat[1][1]; m3x3.m_mat[1][2] = m_mat[1][2];
        m3x3.m_mat[2][0] = m_mat[2][0]; m3x3.m_mat[2][1] = m_mat[2][1]; m3x3.m_mat[2][2] = m_mat[2][2];
    }
    MATH_GENERIC void Matrix4x4<T>::ExtractAxies(Vector3<T>& outX, Vector3<T>& outY, Vector3<T>& outZ)
    {
        outX = Vector3<float>(m_mat[0][0], m_mat[1][0], m_mat[2][0]);
        outX.Normalize();
        outY = Vector3<float>(m_mat[0][1], m_mat[1][1], m_mat[2][1]);
        outY.Normalize();
        outZ = Vector3<float>(m_mat[0][2], m_mat[1][2], m_mat[2][2]);
        outZ.Normalize();
    }
    MATH_GENERIC void Matrix4x4<T>::MakeTransform(const Vector3<T>& position, const Vector3<T>& scale, const Quaternion<T>& rotation)
    {            // quaternion to rotate matrix
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
    }
    MATH_GENERIC void Matrix4x4<T>::MakeInverseTransform(const Vector3<T>& position, const Vector3<T>& scale, const Quaternion<T>& rotation)
    {            // Invert the parameters
        Vector3<float>    inv_translate = -position;
        Vector3<float>    inv_scale(1 / scale.x, 1 / scale.y, 1 / scale.z);
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
    }
    MATH_GENERIC void Matrix4x4<T>::Decomposition(Vector3<T>& position, Vector3<T>& scale, Quaternion<T>& rotate) const
	{
        // Factor M = QR = QDU where Q is orthogonal, D is diagonal,
        // and U is upper triangular with ones on its diagonal.  Algorithm uses
        // Gram-Schmidt orthogonalization (the QR algorithm).
        //
        // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
        //
        //   q0 = m0/|m0|
        //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
        //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
        //
        // where |V| indicates length of vector V and A*B indicates dot
        // product of vectors A and B.  The matrix R has entries
        //
        //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
        //   r10 = 0      r11 = q1*m1  r12 = q1*m2
        //   r20 = 0      r21 = 0      r22 = q2*m2
        //
        // so D = diag(r00,r11,r22) and U has entries u01 = r01/r00,
        // u02 = r02/r00, and u12 = r12/r11.

        // Q = rotation
        // D = scaling
        // U = shear

        // D stores the three diagonal entries r00, r11, r22
        // U stores the entries U[0] = u01, U[1] = u02, U[2] = u12

        // build orthogonal matrix Q
        Matrix3x3 out_Q;

        float inv_length = m_mat[0][0] * m_mat[0][0] + m_mat[1][0] * m_mat[1][0] + m_mat[2][0] * m_mat[2][0];
        if (inv_length != 0)
            inv_length = 1 / std::sqrt(inv_length);

        out_Q[0][0] = m_mat[0][0] * inv_length;
        out_Q[1][0] = m_mat[1][0] * inv_length;
        out_Q[2][0] = m_mat[2][0] * inv_length;

        float dot = out_Q[0][0] * m_mat[0][1] + out_Q[1][0] * m_mat[1][1] + out_Q[2][0] * m_mat[2][1];
        out_Q[0][1] = m_mat[0][1] - dot * out_Q[0][0];
        out_Q[1][1] = m_mat[1][1] - dot * out_Q[1][0];
        out_Q[2][1] = m_mat[2][1] - dot * out_Q[2][0];
        inv_length = out_Q[0][1] * out_Q[0][1] + out_Q[1][1] * out_Q[1][1] + out_Q[2][1] * out_Q[2][1];
        if (inv_length != 0)
            inv_length = 1 / std::sqrt(inv_length);

        out_Q[0][1] *= inv_length;
        out_Q[1][1] *= inv_length;
        out_Q[2][1] *= inv_length;

        dot = out_Q[0][0] * m_mat[0][2] + out_Q[1][0] * m_mat[1][2] + out_Q[2][0] * m_mat[2][2];
        out_Q[0][2] = m_mat[0][2] - dot * out_Q[0][0];
        out_Q[1][2] = m_mat[1][2] - dot * out_Q[1][0];
        out_Q[2][2] = m_mat[2][2] - dot * out_Q[2][0];
        dot = out_Q[0][1] * m_mat[0][2] + out_Q[1][1] * m_mat[1][2] + out_Q[2][1] * m_mat[2][2];
        out_Q[0][2] -= dot * out_Q[0][1];
        out_Q[1][2] -= dot * out_Q[1][1];
        out_Q[2][2] -= dot * out_Q[2][1];
        inv_length = out_Q[0][2] * out_Q[0][2] + out_Q[1][2] * out_Q[1][2] + out_Q[2][2] * out_Q[2][2];
        if (inv_length != 0)
            inv_length = 1 / std::sqrt(inv_length);

        out_Q[0][2] *= inv_length;
        out_Q[1][2] *= inv_length;
        out_Q[2][2] *= inv_length;

        // guarantee that orthogonal matrix has determinant 1 (no reflections)
        float det = out_Q[0][0] * out_Q[1][1] * out_Q[2][2] + out_Q[0][1] * out_Q[1][2] * out_Q[2][0] +
            out_Q[0][2] * out_Q[1][0] * out_Q[2][1] - out_Q[0][2] * out_Q[1][1] * out_Q[2][0] -
            out_Q[0][1] * out_Q[1][0] * out_Q[2][2] - out_Q[0][0] * out_Q[1][2] * out_Q[2][1];

        if (det < 0.0)
        {
            for (int row_index = 0; row_index < 3; row_index++)
                for (int rol_index = 0; rol_index < 3; rol_index++)
                    out_Q[row_index][rol_index] = -out_Q[row_index][rol_index];
        }

        // build "right" matrix R
        Matrix3x3 R;
        R[0][0] = out_Q[0][0] * m_mat[0][0] + out_Q[1][0] * m_mat[1][0] + out_Q[2][0] * m_mat[2][0];
        R[0][1] = out_Q[0][0] * m_mat[0][1] + out_Q[1][0] * m_mat[1][1] + out_Q[2][0] * m_mat[2][1];
        R[1][1] = out_Q[0][1] * m_mat[0][1] + out_Q[1][1] * m_mat[1][1] + out_Q[2][1] * m_mat[2][1];
        R[0][2] = out_Q[0][0] * m_mat[0][2] + out_Q[1][0] * m_mat[1][2] + out_Q[2][0] * m_mat[2][2];
        R[1][2] = out_Q[0][1] * m_mat[0][2] + out_Q[1][1] * m_mat[1][2] + out_Q[2][1] * m_mat[2][2];
        R[2][2] = out_Q[0][2] * m_mat[0][2] + out_Q[1][2] * m_mat[1][2] + out_Q[2][2] * m_mat[2][2];

        // the scaling component
        scale.x = R[0][0];
        scale.y = R[1][1];
        scale.z = R[2][2];

        position = Vector3<float>(m_mat[0][3], m_mat[1][3], m_mat[2][3]);

        MatrixToQuaternion(out_Q, rotate);
        // the shear component
        //float inv_d0 = 1.0f / out_D[0];
        //out_U[0] = R[0][1] * inv_d0;
        //out_U[1] = R[0][2] * inv_d0;
        //out_U[2] = R[1][2] / out_D[1];            
	}
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::Inverse() const
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
    MATH_GENERIC Vector3<T> Matrix4x4<T>::TransformCoord(const Vector3<T>& v)
    {
        Vector4<float> temp(v, 1.0f);
        Vector4<float> ret = (*this) * temp;
        if (ret.w == 0.0f)
        {
            return Vector3<float>{};
        }
        else
        {
            ret /= ret.w;
            return Vector3<float>(ret.x, ret.y, ret.z);
        }
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::ViewMatrix(const Vector3<T>& position, const Quaternion<T>& orientation, const Matrix4x4<T>* reflectMat)
    {
        Matrix4x4 viewMatrix;

        // View matrix is:
        //
        //  [ Lx  Uy  Dz  Tx  ]
        //  [ Lx  Uy  Dz  Ty  ]
        //  [ Lx  Uy  Dz  Tz  ]
        //  [ 0   0   0   1   ]
        //
        // Where T = -(Transposed(Rot) * Pos)

        // This is most efficiently done using 3x3 Matrices
        Matrix3x3 rot;
        QuaternionToMatrix(orientation, rot);

        // Make the translation relative to new axes
        Matrix3x3 rotT = rot.Transpose();
        Vector3<float>   trans = -rotT * position;

        // Make final matrix
        viewMatrix = Matrix4x4::IDENTITY;
        viewMatrix.SetMatrix3x3(rotT); // fills upper 3x3
        viewMatrix[0][3] = trans.x;
        viewMatrix[1][3] = trans.y;
        viewMatrix[2][3] = trans.z;

        // Deal with reflections
        if (reflectMat)
        {
            viewMatrix = viewMatrix * (*reflectMat);
        }

        return viewMatrix;
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::LookAtMatrix(const Vector3<T>& eye, const Vector3<T>& at, const Vector3<T>& up)
    {
        const Vector3<float>& upNormalized = up.NormalizeCopy();

        Vector3<float> f = (at - eye);
        f.Normalize();
        Vector3<float> s = Vector3<float>::Cross(f, upNormalized);
        s.Normalize();
        Vector3<float> u = Vector3<float>::Cross(s, f);

        Matrix4x4<T> view_mat = Matrix4x4<T>::IDENTITY;
        view_mat[0][0] = s.x;
        view_mat[0][1] = s.y;
        view_mat[0][2] = s.z;
        view_mat[0][3] = -s.Dot(eye);
        view_mat[1][0] = u.x;
        view_mat[1][1] = u.y;
        view_mat[1][2] = u.z;
        view_mat[1][3] = -u.Dot(eye);
        view_mat[2][0] = -f.x;
        view_mat[2][1] = -f.y;
        view_mat[2][2] = -f.z;
        view_mat[2][3] = f.Dot(eye);
        return view_mat;
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::PerspectiveMatrix(T fovy, T aspect, T zNear, T zfar)
    {
        return Matrix4x4<T>();
    }
    MATH_GENERIC Matrix4x4<T> Matrix4x4<T>::OrthographicMatrix(T left, T right, T bottom, T top, T znear, T zfar)
    {
        float inv_width = 1.0f / (right - left);
        float inv_height = 1.0f / (top - bottom);
        float inv_distance = 1.0f / (zfar - znear);

        float A = 2 * inv_width;
        float B = 2 * inv_height;
        float C = -(right + left) * inv_width;
        float D = -(top + bottom) * inv_height;
        float q = -2 * inv_distance;
        float qn = -(zfar + znear) * inv_distance;

        // NB: This creates 'uniform' orthographic projection matrix,
        // which depth range [-1,1], right-handed rules
        //
        // [ A   0   0   C  ]
        // [ 0   B   0   D  ]
        // [ 0   0   q   qn ]
        // [ 0   0   0   1  ]
        //
        // A = 2 * / (right - left)
        // B = 2 * / (top - bottom)
        // C = - (right + left) / (right - left)
        // D = - (top + bottom) / (top - bottom)
        // q = - 2 / (far - near)
        // qn = - (far + near) / (far - near)

        Matrix4x4<T> proj_matrix = Matrix4x4<T>::ZERO;
        proj_matrix[0][0] = A;
        proj_matrix[0][3] = C;
        proj_matrix[1][1] = B;
        proj_matrix[1][3] = D;
        proj_matrix[2][2] = q;
        proj_matrix[2][3] = qn;
        proj_matrix[3][3] = 1;

        return proj_matrix;
    }
}