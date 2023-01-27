#pragma once
#include "RenderMesh.h"
namespace Engine {
	bool LoadMeshFromGLTF(const char* file, TVector<SPrimitiveData>& primitives);
	bool LoadMeshFromFBX (const char* file, TVector<SPrimitiveData>& primitives);

}