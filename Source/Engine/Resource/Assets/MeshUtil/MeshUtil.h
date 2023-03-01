#pragma once
#include "Core/Container/Container.h"
struct SPrimitiveData;

bool LoadMeshFromGLTF(const char* file, TVector<SPrimitiveData>& primitives);
bool LoadMeshFromFBX (const char* file, TVector<SPrimitiveData>& primitives);