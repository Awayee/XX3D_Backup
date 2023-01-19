#pragma once
#include "Core/Container/Container.h"
#include "Core/Math/CoreMath.h"
#include "RHI/RHI.h"
namespace Engine {

	typedef uint32_t IndexType;

	struct Vertex {
		MATH::Vector3 position;
		MATH::Vector3 normal;
		MATH::Vector3 tangent;
		MATH::Vector2 uv;
		static RHI::RVertexInputBinding GetInputBindings();
		static TVector<RHI::RVertexInputAttribute> GetInputAttributes();
	};

	class Primitive {
	private:
		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		RHI::RBuffer* m_VertexBuffer;
		RHI::RMemory* m_VertexBufferMemory;
		RHI::RBuffer* m_IndexBuffer;
		RHI::RMemory* m_IndexBufferMemory;
	public:
		Primitive(const TVector<Vertex>& vertices, const TVector<IndexType>& indices);
		~Primitive();
	};
}