#pragma once
#include "Core/Math/Math.h"
#include "Core/Memory/SmartPointer.h"
#include "../Render/RenderResources.h"
#include "../Render/RenderCommon.h"
#include "../Scene//RenderScene.h"

namespace Engine {
	struct SPrimitiveData{
		TVector<Vertex> vertices;
		TVector<uint32> indices;
	};

	// static mesh
	class RenderMesh: public RenderObject {
	private:
		TVector<TUniquePtr<Primitive>> m_Primitives;
		Math::FMatrix4x4 m_TransformMat{ Math::FMatrix4x4::IDENTITY };
		BufferCommon m_TransformUniform;
		Math::FVector4 m_MaterialParam{ 1.0f, 1.0f, 1.0f, 1.0f };
		BufferCommon m_MaterialUniform;
		RHI::RDescriptorSet* m_TransformDescs{nullptr};
		RHI::RDescriptorSet* m_MaterialDescs { nullptr };
		//RHI::RDescriptorSet* m_DescriptorSet;
	private:
		void LoadPrimitives(const TVector<SPrimitiveData>& primitives);
	public:
		RenderMesh(const TVector<SPrimitiveData>& primitives, RenderScene* scene);
		void DrawCall(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout) override;
		const TVector<TUniquePtr<Primitive>>& GetPrimitives() const { return m_Primitives; }
		RHI::RDescriptorSet* GetDescriptorSet()const { return m_TransformDescs; }
		~RenderMesh();
	};
}