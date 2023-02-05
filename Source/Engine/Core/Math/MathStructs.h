#pragma once
#include "Vector.h"

namespace Math {
	struct AxisAlignedBox {
	public:
		FVector3 center {0.0f, 0.0f, 0.0f};
		FVector3 extent {0.0f, 0.0f, 0.0f};
		AxisAlignedBox() = default;
		AxisAlignedBox(const FVector3& center, const FVector3& extent);
		void Contain(const FVector3& newPoint);
		void Combine(const AxisAlignedBox& box);
	};
}