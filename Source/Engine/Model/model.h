#pragma once
#include "../Mesh/mesh.h"
#include "Core/Container/TArray.h"

namespace Engine {
	class Model {
	private:
		TArray<Mesh> m_Meshes;
		void LoadFromGLTF(const char* file);
		void LoadFromFbx(const char* fbx);

	public:
		Model(const char* file);
		Model(const char* file, MATH::Vector3& pos);
		~Model();
	};
}
