#pragma once
#include "../Mesh/RenderMesh.h"
#include "Core/Container/TArray.h"

namespace Engine {
	class Model {
	private:
		TArray<RenderMesh> m_Meshes;
		void LoadFromGLTF(const char* file);
		void LoadFromFbx(const char* fbx);

	public:
		Model(const char* file);
		Model(const char* file, Math::FVector3& pos);
		~Model();
	};
}
