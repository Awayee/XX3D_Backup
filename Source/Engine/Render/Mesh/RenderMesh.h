#pragma once
#include "Core/Math/Math.h"
#include "Core/Memory/SmartPointer.h"
#include "Core/Container/String.h"
#include "Render/Common/RenderResources.h"
#include "Render/Common/RenderCommon.h"
#include "Render/Scene//RenderScene.h"
#include "Resource/Assets/AssetData.h"

namespace Engine {
	// static mesh
	class RenderMesh: public RenderObject {
	private:
		TVector<TUniquePtr<Primitive>> m_Primitives;
		TVector<Material*> m_Materials;
		Math::FMatrix4x4 m_TransformMat{ Math::FMatrix4x4::IDENTITY };
		BufferCommon m_TransformUniform;
		RHI::RDescriptorSet* m_TransformDescs{nullptr};
		//RHI::RDescriptorSet* m_DescriptorSet;
	private:
		void LoadPrimitives(const TVector<SPrimitiveData>& primitives);
	public:
		RenderMesh(const TVector<SPrimitiveData>& primitives, RenderScene* scene);
		void DrawCall(RHI::RCommandBuffer* cmd, RHI::RPipelineLayout* layout) override;
		RHI::RDescriptorSet* GetDescriptorSet()const { return m_TransformDescs; }
		~RenderMesh();
	};
}