#pragma once
#include "Core/Container/Container.h"
#include "Core/Math/Math.h"
#include "RHI/RHI.h"
#include "RenderCommon.h"
namespace Engine {

	typedef uint32 IndexType;

	struct Vertex {
		Math::FVector3 position;
		Math::FVector3 normal;
		Math::FVector3 tangent;
		Math::FVector2 uv;
		static TVector<RHI::RVertexInputBinding> GetInputBindings();
		static TVector<RHI::RVertexInputAttribute> GetInputAttributes();
	};

	class Primitive {
	private:
		uint32 m_VertexCount;
		uint32 m_IndexCount;
		TUniquePtr<BufferCommon> m_Vertex;
		TUniquePtr<BufferCommon> m_Index;
	public:
		Primitive(const TVector<Vertex>& vertices, const TVector<IndexType>& indices);
		RHI::RBuffer* GetVertexBuffer() const { return m_Vertex ? m_Vertex->Buffer : nullptr; }
		RHI::RBuffer* GetIndexBuffer()  const { return m_Index  ? m_Index->Buffer  : nullptr; }
		uint32 GetVertexCount()const { return m_VertexCount; }
		uint32 GetIndexCount()const { return m_IndexCount; }
		~Primitive();
	};

	void DrawPrimitive(RHI::RCommandBuffer* cmd, const Primitive* primitive);

	class Quad {
		BufferCommon m_VertexBuffer;
	public:
		Quad();
		~Quad();
		RHI::RBuffer* GetVertexBuffer() const { return m_VertexBuffer.Buffer; }
	};

	void DrawQuad(RHI::RCommandBuffer* cmd, const Quad* quad);
}