#pragma once
#include "Core/Math/CoreMath.h"
#include "Core/Memory/SmartPointer.h"
#include "Engine/Render/RenderResources.h"

namespace Engine {
	struct SPrimitiveData{
		TVector<Vertex> vertices;
		TVector<uint32_t> indices;
	};

	// static mesh
	class Mesh {
	private:
		TVector<TUniquePtr<Primitive>> m_Primitives;
		//RHI::RDescriptorSet* m_DescriptorSet;
	public:
		Mesh() = default;
		Mesh(const TVector<SPrimitiveData>& primitives) { LoadPrimitives(primitives); };
		void LoadPrimitives(const TVector<SPrimitiveData>& primitives);
		~Mesh();
	};
}