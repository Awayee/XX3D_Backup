#include "MathStructs.h"

namespace Math {
	AxisAlignedBox::AxisAlignedBox(const FVector3& center, const FVector3& extent) : center(center), extent(extent) {}

	void AxisAlignedBox::Contain(const FVector3& newPoint)
	{
		FVector3 min = center - extent;
		FVector3 max = center + extent;
		min = FVector3::Min(min, newPoint);
		max = FVector3::Max(max, newPoint);
		center = (min + max) * 0.5f;
		extent = (max - min) * 0.5f;
	}
	void AxisAlignedBox::Combine(const AxisAlignedBox& box)
	{
		FVector3 thisMin = center - extent;
		FVector3 thisMax = center + extent;
		FVector3 otherMin = box.center - box.extent;
		FVector3 otherMax = box.center + box.extent;
		thisMin = FVector3::Min(thisMin, otherMin);
		thisMax = FVector3::Max(thisMax, otherMax);
		center = (thisMin + thisMax) * 0.5f;
		extent = (thisMax - thisMin) * 0.5f;
	}
}