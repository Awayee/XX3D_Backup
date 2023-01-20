#pragma once
#include "Core/Container/Container.h"
#include "Core/Math/Math.h"
#include "RHI/RHI.h"
namespace Engine {

	typedef uint32_t IndexType;

	struct Vertex {
		Math::FVector3 position;
		Math::FVector3 normal;
		Math::FVector3 tangent;
		Math::FVector2 uv;
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
		RHI::RBuffer* GetVertexBuffer() const { return m_VertexBuffer; }
		RHI::RBuffer* GetIndexBuffer()  const { return m_IndexBuffer; }
		uint32_t GetVertexCount()const { return m_VertexCount; }
		uint32_t GetIndexCount()const { return m_IndexCount; }
		~Primitive();
	};
}