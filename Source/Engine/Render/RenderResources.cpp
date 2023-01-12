#include "RenderResources.h"

namespace Engine {
	RHI::RVertexInputBinding SVertex::GetInputBindings() { return { 0, sizeof(SVertex), RHI::VERTEX_INPUT_RATE_VERTEX }; }

	TVector<RHI::RVertexInputAttribute> SVertex::GetInputAttributes()
	{
		TVector<RHI::RVertexInputAttribute> attributes(3);
		attributes[0] = { 0, 0, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(SVertex, position) };
		attributes[1] = { 0, 1, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(SVertex, normal) };
		attributes[2] = { 0, 2, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(SVertex, tangent) };
		attributes[3] = { 0, 3, RHI::FORMAT_R32G32_SFLOAT,    offsetof(SVertex, uv) };
		return attributes;
	}
}