#include "Quaternion.h"
#include <cmath>
namespace MATH {
	void Quaternion::FromAngleAxis(float a, const Vector3& axis)
	{
        float half_a(0.5f * a);
        float sin_v = std::sin(half_a);
        w = std::cos(half_a);
        x = axis.x * sin_v;
        y = axis.y * sin_v;
        z = axis.z * sin_v;
	}
    Vector3 Quaternion::RotateVector3(const Vector3& v) const
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
}