#include "RenderMesh.h"

namespace Engine {
	RenderMesh::RenderMesh(const TVector<SPrimitiveData>& primitives, RenderScene* scene) : RenderObject(scene)
	{
		LoadPrimitives(primitives);
	}

	void RenderMesh::LoadPrimitives(const TVector<SPrimitiveData>& primitives)
	{
		m_Primitives.resize(primitives.size());
		for (uint32 i = 0; i < primitives.size(); ++i) {
			auto& primitiveData = primitives[i];
			m_Primitives[i].reset(new Primitive(primitiveData.vertices, primitiveData.indices));
		}
		// uniform
		m_UniformBuffer.Create(sizeof(Math::FMatrix4x4), RHI::BUFFER_USAGE_UNIFORM_BUFFER_BIT, RHI::MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr);
		// descriptor set
		RHI::RDescriptorSetLayout* layout = DescsMgr::GetLayout(DESCS_MODEL);
		m_DescriptorSet = RHI_INSTANCE->AllocateDescriptorSet(layout);
		RHI::RDescriptorInfo info{};
		info.buffer = m_UniformBuffer.Buffer;
		info.offset = 0;
		info.range = sizeof(Math::FMatrix4x4);
		RHI_INSTANCE->UpdateDescriptorSet(m_DescriptorSet, 0, 0, 1, RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, info);
	}

	void RenderMesh::DrawCall(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout)
	{
		GET_RHI(rhi);
		rhi->CmdBindDescriptorSet(cmd, RHI::PIPELINE_GRAPHICS, layout, m_DescriptorSet, 0);
		for(auto& primitive: m_Primitives) {
			DrawPrimitive(cmd, primitive.get());
		}
	}

	RenderMesh::~RenderMesh()
	{
		m_Primitives.clear();
		m_UniformBuffer.Release();
		RHI_INSTANCE->FreeDescriptorSet(m_DescriptorSet);
	}
}