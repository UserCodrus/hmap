#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

struct ComponentData
{
	ComponentData(unsigned size);

	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
};