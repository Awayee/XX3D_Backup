#pragma once
#include "Core/Container/Container.h"
#include "Core/Container/String.h"
#include "Core/Math/Math.h"

struct FVertex {
	Math::FVector3 Position;
	Math::FVector2 UV;
	Math::FVector3 Normal;
	Math::FVector3 Tangent;
};

typedef uint32 IndexType;

struct SPrimitiveData {
	TVector<FVertex> Vertices;
	TVector<uint32>  Indices;
	TVector<String>  Textures;
};