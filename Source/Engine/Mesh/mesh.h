#pragma once
#include "Core/Math/CoreMath.h"

namespace Engine {

	struct Vertex {
		MATH::Vector3 vertex;
		MATH::Vector3 normal;
		MATH::Vector2 uv;
	};

	struct Mesh {
		MATH::Vector3 position, rotation, scale;
		MATH::Matrix4x4 transformMatrix;
		uint32_t vertexCount;
		uint32_t indexCount;
	};
}