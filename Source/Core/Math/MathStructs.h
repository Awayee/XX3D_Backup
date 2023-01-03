#pragma once
#include "Vector.h"

namespace MATH {
	struct AxisAlignedBox {
	public:
		Vector3 center {0.0f, 0.0f, 0.0f};
		Vector3 extent {0.0f, 0.0f, 0.0f};
		AxisAlignedBox() = default;
		AxisAlignedBox(const Vector3& center, const Vector3& extent);
		void Contain(const Vector3& newPoint);
		void Combine(const AxisAlignedBox& box);
	};
}