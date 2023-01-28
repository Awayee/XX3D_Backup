#include "RenderResources.h"
#include "RHI/RHI.h"

namespace Engine {
	TVector<RHI::RVertexInputBinding> Vertex::GetInputBindings() { return { { 0, sizeof(Vertex), RHI::VERTEX_INPUT_RATE_VERTEX } }; }

	TVector<RHI::RVertexInputAttribute> Vertex::GetInputAttributes()
	{
		TVector<RHI::RVertexInputAttribute> attributes;
		attributes.push_back({ 0, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position)});
		attributes.push_back({ 0, RHI::FORMAT_R32G32_SFLOAT,    offsetof(Vertex, uv)});
		attributes.push_back({ 0, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)});
		attributes.push_back({ 0, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent)});
		return attributes;
	}

	Primitive::Primitive(const TVector<Vertex>& vertices, const TVector<IndexType>& indices)
	{
		m_VertexCount = vertices.size();
		m_IndexCount  = indices.size();
		if(0 == m_VertexCount) {
			return;
		}
		GET_RHI(rhi);
		uint32 bufferSize = m_VertexCount * sizeof(Vertex);
		m_Vertex.reset(new BufferCommon); m_Vertex->CreateForVertex(bufferSize);

		BufferCommon vertexStaging;
		vertexStaging.CreateForStaging(bufferSize, (void*)vertices.data());

		if(m_IndexCount > 0) {
			bufferSize = m_IndexCount * sizeof(IndexType);
			m_Index.reset(new BufferCommon); m_Index->CreateForIndex(bufferSize);
			
			BufferCommon indexStaging;
			indexStaging.CreateForStaging(bufferSize, (void*)indices.data());
			rhi->ImmediateCommit([this, &vertexStaging, &indexStaging](RHI::RCommandBuffer* cmd) {
				RHI_INSTANCE->CmdCopyBuffer(cmd, vertexStaging.Buffer, m_Vertex->Buffer, 0, 0, vertexStaging.Size);
				RHI_INSTANCE->CmdCopyBuffer(cmd, indexStaging.Buffer, m_Index->Buffer, 0, 0, indexStaging.Size);
			});
			indexStaging.Release();
			vertexStaging.Release();
		}
		else {
			rhi->ImmediateCommit([this, &vertexStaging](RHI::RCommandBuffer* cmd) {
				RHI_INSTANCE->CmdCopyBuffer(cmd, vertexStaging.Buffer, m_Vertex->Buffer, 0, 0, vertexStaging.Size);
			});
			vertexStaging.Release();
		}
	}

	Primitive::~Primitive()
	{
		if (m_Index)m_Index->Release();
		if (m_Vertex)m_Vertex->Release();
	}

	void DrawPrimitive(RHI::RCommandBuffer* cmd, const Primitive* primitive)
	{
		uint32 vertexCount = primitive->GetVertexCount();
		if (0 == vertexCount) {
			return;
		}
		GET_RHI(rhi);
		rhi->CmdBindVertexBuffer(cmd, primitive->GetVertexBuffer(), 0, 0);
		uint32 indexCount = primitive->GetIndexCount();
		if (0 == indexCount) {
			rhi->CmdDraw(cmd, vertexCount, 1, 0, 0);
		}
		else {
			rhi->CmdBindIndexBuffer(cmd, primitive->GetIndexBuffer(), 0);
			rhi->CmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
		}
	}
	Quad::Quad()
	{
		TVector<Math::FVector3> vertices{
			{0, 0, 0}, {1, 0, 0}, {0, 1, 0},
			{0, 1, 0}, {1, 0, 0}, {1, 1, 0}
		};
		uint32 bufferSize = vertices.size() * sizeof(Math::FVector3);
		m_VertexBuffer.Create(bufferSize, RHI::BUFFER_USAGE_VERTEX_BUFFER_BIT | RHI::BUFFER_USAGE_TRANSFER_DST_BIT,
			RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		BufferCommon staging;
		staging.Create(bufferSize, RHI::BUFFER_USAGE_TRANSFER_SRC_BIT,
			RHI::MEMORY_PROPERTY_HOST_COHERENT_BIT | RHI::MEMORY_PROPERTY_HOST_CACHED_BIT, (void*)vertices.data());
		RHI_INSTANCE->ImmediateCommit([&staging, this, bufferSize](RHI::RCommandBuffer* cmd) {
			RHI_INSTANCE->CmdCopyBuffer(cmd, staging.Buffer, m_VertexBuffer.Buffer, 0, 0, bufferSize);
		});
		staging.Release();
	}
	Quad::~Quad()
	{
		m_VertexBuffer.Release();
	}


	void DrawQuad(RHI::RCommandBuffer* cmd, const Quad* quad)
	{
		RHI_INSTANCE->CmdBindVertexBuffer(cmd, quad->GetVertexBuffer(), 0, 0);
		RHI_INSTANCE->CmdDraw(cmd, 6, 1, 0, 0);
	}
}