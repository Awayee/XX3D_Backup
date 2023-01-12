#pragma once
#include "Core/Container/Container.h"
#include "Core/Math/CoreMath.h"
#include "RHI/RHI.h"
namespace Engine {

	struct SVertex {
		MATH::Vector3 position;
		MATH::Vector3 normal;
		MATH::Vector3 tangent;
		MATH::Vector2 uv;
		static RHI::RVertexInputBinding GetInputBindings();
		static TVector<RHI::RVertexInputAttribute> GetInputAttributes();
	};

	struct SPrimitive {
		TVector<SVertex> m_Vertices;
		TVector<uint32_t> m_Indices;
	};
}