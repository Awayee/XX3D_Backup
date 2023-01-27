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

	Primitive::~Primitive()
	{
		if (nullptr != m_VertexBuffer) {
			RHI_INSTANCE->DestroyBuffer(m_VertexBuffer);
			RHI_INSTANCE->FreeMemory(m_VertexBufferMemory);
		}
		if (nullptr != m_IndexBuffer) {
			RHI_INSTANCE->DestroyBuffer(m_IndexBuffer);
			RHI_INSTANCE->FreeMemory(m_IndexBufferMemory);
		}
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
		rhi->CreateBufferWithMemory(bufferSize, RHI::BUFFER_USAGE_VERTEX_BUFFER_BIT | RHI::BUFFER_USAGE_TRANSFER_DST_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			m_VertexBuffer, m_VertexBufferMemory, 0, nullptr);
		RHI::RBuffer* stagingBuffer;
		RHI::RMemory* stagingMemory;
		rhi->CreateBufferWithMemory(bufferSize, RHI::BUFFER_USAGE_TRANSFER_SRC_BIT, RHI::MEMORY_PROPERTY_HOST_COHERENT_BIT | RHI::MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			stagingBuffer, stagingMemory, m_VertexCount * sizeof(Vertex), (void*)vertices.data());
		RHI::RBuffer* stagingBuffer1;
		RHI::RMemory* stagingMemory1;
		if(0 != m_IndexCount) {
			bufferSize = m_IndexCount * sizeof(IndexType);
			rhi->CreateBufferWithMemory(bufferSize, RHI::BUFFER_USAGE_INDEX_BUFFER_BIT | RHI::BUFFER_USAGE_TRANSFER_DST_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_IndexBuffer, m_IndexBufferMemory, 0, nullptr);
			rhi->CreateBufferWithMemory(bufferSize, RHI::BUFFER_USAGE_TRANSFER_SRC_BIT, RHI::MEMORY_PROPERTY_HOST_COHERENT_BIT | RHI::MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				stagingBuffer1, stagingMemory1, bufferSize, (void*)indices.data());
		}
		rhi->ImmediateCommit([this, stagingBuffer, stagingBuffer1](RHI::RCommandBuffer* cmd) {
			RHI_INSTANCE->CmdCopyBuffer(cmd, stagingBuffer, m_VertexBuffer, 0, 0, m_VertexCount * sizeof(Vertex));
			if (stagingBuffer1) {
				RHI_INSTANCE->CmdCopyBuffer(cmd, stagingBuffer1, m_IndexBuffer, 0, 0, m_IndexCount * sizeof(IndexType));
			}
		});
		rhi->DestroyBuffer(stagingBuffer);
		rhi->FreeMemory(stagingMemory);
		if(nullptr != stagingBuffer1) {
			rhi->DestroyBuffer(stagingBuffer1);
			rhi->FreeMemory(stagingMemory1);
		}
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
			rhi->CmdBindIndexBuffer(cmd, primitive->GetIndexBuffer(), 0);
			rhi->CmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
		}
		else {
			rhi->CmdDraw(cmd, vertexCount, 1, 0, 0);
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
		RHI_INSTANCE->ImmediateCommit([staging, this, bufferSize](RHI::RCommandBuffer* cmd) {
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