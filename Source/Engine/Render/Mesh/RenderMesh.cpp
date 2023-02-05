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
		m_TransformUniform.CreateForUniform(sizeof(Math::FMatrix4x4), &m_TransformMat);
		m_MaterialUniform.CreateForUniform(sizeof(Math::FVector4), &m_MaterialParam);
		// descriptor set
		m_TransformDescs = RHI_INSTANCE->AllocateDescriptorSet(DescsMgr::Get(DESCS_MODEL));
		m_TransformDescs->UpdateUniformBuffer(0, m_TransformUniform.Buffer);
		//RHI::RDescriptorInfo info{};
		//info.buffer = m_TransformUniform.Buffer;
		//info.offset = 0;
		//info.range = m_TransformUniform.Size;
		//RHI_INSTANCE->UpdateDescriptorSet(m_TransformDescs, 0, 0, 1, RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, info);

		// TODO to material obj
		m_MaterialDescs = RHI_INSTANCE->AllocateDescriptorSet(DescsMgr::Get(DESCS_MATERIAL));
		m_MaterialDescs->UpdateUniformBuffer(0, m_MaterialUniform.Buffer);
		//info.buffer = m_MaterialUniform.Buffer;
		//info.offset = 0;
		//info.range = m_MaterialUniform.Size;
		//RHI_INSTANCE->UpdateDescriptorSet(m_MaterialDescs, 0, 0, 1, RHI::DESCRIPTOR_TYPE_UNIFORM_BUFFER, info);
	}

	void RenderMesh::DrawCall(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout)
	{
		cmd->BindDescriptorSet(layout, m_TransformDescs, 1, RHI::PIPELINE_GRAPHICS);
		cmd->BindDescriptorSet(layout, m_MaterialDescs,  2, RHI::PIPELINE_GRAPHICS);
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