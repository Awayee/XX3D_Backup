#include "MathStructs.h"

namespace MATH {
	AxisAlignedBox::AxisAlignedBox(const Vector3& center, const Vector3& extent) : center(center), extent(extent) {}

	void AxisAlignedBox::Contain(const Vector3& newPoint)
	{
		Vector3 min = center - extent;
		Vector3 max = center + extent;
		min = Vector3::Min(min, newPoint);
		max = Vector3::Max(max, newPoint);
		center = (min + max) * 0.5f;
		extent = (max - min) * 0.5f;
	}
	void AxisAlignedBox::Combine(const AxisAlignedBox& box)
	{
		Vector3 thisMin = center - extent;
		Vector3 thisMax = center + extent;
		Vector3 otherMin = box.center - box.extent;
		Vector3 otherMax = box.center + box.extent;
		thisMin = Vector3::Min(thisMin, otherMin);
		thisMax = Vector3::Max(thisMax, otherMax);
		center = (thisMin + thisMax) * 0.5f;
		extent = (thisMax - thisMin) * 0.5f;
	}
}