#include "RenderMesh.h"

namespace Engine {
	RenderMesh::RenderMesh(const TVector<SPrimitiveData>& primitives, RenderScene* scene) : RenderObject(scene)
	{
		LoadPrimitives(primitives);
	}

	void RenderMesh::LoadPrimitives(const TVector<SPrimitiveData>& primitives)
	{
		// primitives
		m_Primitives.resize(primitives.size());
		for (uint32 i = 0; i < primitives.size(); ++i) {
			auto& primitiveData = primitives[i];
			m_Primitives[i].reset(new Primitive(primitiveData.vertices, primitiveData.indices));
		}

		// uniform
		m_TransformUniform.CreateForUniform(sizeof(Math::FMatrix4x4));
		m_MaterialUniform.CreateForUniform(sizeof(Math::FVector4));
		// descriptor set
		m_TransformDescs = RHI_INSTANCE->AllocateDescriptorSet(DescsMgr::GetLayout(DESCS_MODEL));
		RHI::RDescriptorInfo info{};
		info.buffer = m_TransformUniform.Buffer;
		info.offset = 0;
		info.range = m_TransformUniform.Size;
		RHI_INSTANCE->UpdateDescriptorSet(m_TransformDescs, 0, 0, 1, RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, info);

		m_MaterialDescs = RHI_INSTANCE->AllocateDescriptorSet(DescsMgr::GetLayout(DESCS_MATERIAL));
		info.buffer = m_MaterialUniform.Buffer;
		info.offset = 0;
		info.range = m_MaterialUniform.Size;
		RHI_INSTANCE->UpdateDescriptorSet(m_MaterialDescs, 0, 0, 1, RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, info);
	}

	void RenderMesh::DrawCall(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout)
	{
		GET_RHI(rhi);
		rhi->CmdBindDescriptorSet(cmd, RHI::PIPELINE_GRAPHICS, layout, m_TransformDescs, 1);
		rhi->CmdBindDescriptorSet(cmd, RHI::PIPELINE_GRAPHICS, layout, m_MaterialDescs,  2);
		for(auto& primitive: m_Primitives) {
			DrawPrimitive(cmd, primitive.get());
		}
	}

	RenderMesh::~RenderMesh()
	{
		m_Primitives.clear();
		m_TransformUniform.Release();
		m_MaterialUniform.Release();
		//RHI_INSTANCE->FreeDescriptorSet(m_TransformDescs);
	}
}