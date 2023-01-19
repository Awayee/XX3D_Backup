#pragma once
#include "Mesh.h"
namespace Engine {
	bool LoadMeshFromGLTF(const char* file, Mesh* mesh);
	bool LoadMeshFromFBX(const char* file, Mesh* mesh);

}