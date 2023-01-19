#include "Mesh.h"

namespace Engine {

	void Mesh::LoadPrimitives(const TVector<SPrimitiveData>& primitives)
	{
		m_Primitives.resize(primitives.size());
		for (uint32_t i = 0; i < primitives.size(); ++i) {
			auto& primitiveData = primitives[i];
			m_Primitives[i].reset(new Primitive(primitiveData.vertices, primitiveData.indices));
		}
	}

	Mesh::~Mesh()
	{
	}
}