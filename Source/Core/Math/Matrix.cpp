#include "Matrix.h"
#include "Core/macro.h"
#include <cmath>

namespace MATH {
	Matrix3x3::Matrix3x3(float arr[3][3])
	{
		memcpy(m_mat[0], arr[0], 3 * sizeof(float));
		memcpy(m_mat[1], arr[1], 3 * sizeof(float));
		memcpy(m_mat[2], arr[2], 3 * sizeof(float));
	}
    bool Matrix3x3::Inverse(Matrix3x3& inv_mat, float fTolerance) const
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
        for (size_t row_index = 0; row_index < 3; row_index++)
        {
            for (size_t col_index = 0; col_index < 3; col_index++)
                inv_mat[row_index][col_index] *= inv_det;
        }

        return true;
    }

    const Matrix4x4 Matrix4x4::IDENTITY(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	float* Matrix4x4::operator[](size_t row_index)
	{
		ASSERT(row_index < 4);
		return m_mat[row_index];
	}
	const float* Matrix4x4::operator[](size_t row_index) const
	{
		ASSERT(row_index < 4);
		return m_mat[row_index];
	}
	Matrix4x4 Matrix4x4::RotationMatrix(Vector3 normal)
	{
		Vector3 up = Vector3(0, 0, 1);
		if (fabs(normal.z) > 0.999f)
		{
			up = Vector3(0, 1, 0);
		}

		Vector3 left = up.Cross(normal);
		up = normal.Cross(left);

		left.Normalize();
		up.Normalize();

		Matrix4x4 result = Matrix4x4::IDENTITY;
		result.SetMatrix3x3(Matrix3x3(left, up, normal));

		return result.Transpose();
	}
	void Matrix4x4::Decomposition(Vector3& position, Vector3& scale, Quaternion& rotate) const
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
            for (size_t row_index = 0; row_index < 3; row_index++)
                for (size_t rol_index = 0; rol_index < 3; rol_index++)
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

        position = Vector3(m_mat[0][3], m_mat[1][3], m_mat[2][3]);

        MatrixToQuaternion(out_Q, rotate);
        // the shear component
        //float inv_d0 = 1.0f / out_D[0];
        //out_U[0] = R[0][1] * inv_d0;
        //out_U[1] = R[0][2] * inv_d0;
        //out_U[2] = R[1][2] / out_D[1];            
	}
    void MatrixToQuaternion(const Matrix3x3& m, Quaternion& q)
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
            size_t s_iNext[3] = { 1, 2, 0 };
            size_t i = 0;
            if (m[1][1] > m[0][0])
                i = 1;
            if (m[2][2] > m[i][i])
                i = 2;
            size_t j = s_iNext[i];
            size_t k = s_iNext[j];

            root = std::sqrt(m[i][i] - m[j][j] - m[k][k] + 1.0f);
            float* apkQuat[3] = { &q.x, &q.y, &q.z };
            *apkQuat[i] = 0.5f * root;
            root = 0.5f / root;
            q.w = (m[k][j] - m[j][k]) * root;
            *apkQuat[j] = (m[j][i] + m[i][j]) * root;
            *apkQuat[k] = (m[k][i] + m[i][k]) * root;
        }
    }
}