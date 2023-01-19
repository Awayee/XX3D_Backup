#include "RenderResources.h"
#include "RHI/RHIInstance.h"

namespace Engine {
	RHI::RVertexInputBinding Vertex::GetInputBindings() { return { 0, sizeof(Vertex), RHI::VERTEX_INPUT_RATE_VERTEX }; }

	TVector<RHI::RVertexInputAttribute> Vertex::GetInputAttributes()
	{
		TVector<RHI::RVertexInputAttribute> attributes(3);
		attributes[0] = { 0, 0, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) };
		attributes[1] = { 0, 1, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) };
		attributes[2] = { 0, 2, RHI::FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, tangent) };
		attributes[3] = { 0, 3, RHI::FORMAT_R32G32_SFLOAT,    offsetof(Vertex, uv) };
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
		size_t bufferSize = m_VertexCount * sizeof(Vertex);
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
			rhi->CreateBufferWithMemory(bufferSize, RHI::BUFFER_USAGE_TRANSFER_DST_BIT, RHI::MEMORY_PROPERTY_HOST_COHERENT_BIT | RHI::MEMORY_PROPERTY_HOST_VISIBLE_BIT,
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
		if(stagingBuffer1) {
			rhi->DestroyBuffer(stagingBuffer1);
			rhi->FreeMemory(stagingMemory1);
		}
	}
}