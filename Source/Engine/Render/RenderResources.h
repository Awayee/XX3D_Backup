#pragma once
#include "Core/Container/Container.h"
#include "Core/Math/CoreMath.h"
namespace Engine {

	struct SVertex {
		MATH::Vector3 vertex;
		MATH::Vector3 normal;
		MATH::Vector2 uv;
	};

	struct SPrimitive {
		TVector<SVertex> m_Vertices;
		TVector<uint32_t> m_Indices;
	};
}