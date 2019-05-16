// Copyright © 2019 Created by Brian Faubion

#include "Terrain.h"

#include "Engine/Engine.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Kismet/KismetRenderingLibrary.h"
#include "Runtime/Engine/Classes/Materials/Material.h"

// The largest value stored in the heightmap
#define HMAPMAX 65535
// The median value of the heightmap
#define HMAPMED 32767

ATerrain::ATerrain()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	// Create the procedural mesh component
	Mesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	Mesh->SetupAttachment(RootComponent);

	// Scale the terrain so that 1 square = 1 units
	SetActorRelativeScale3D(FVector(100.0f, 100.0f, 100.0f));

	RebuildHeightmap();
}

void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//mesh->ClearMeshSection(0);
}

void ATerrain::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FString name = PropertyChangedEvent.MemberProperty->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "Property Changed: " + name);

	// If the mesh properties change, rebuild the heightmap and mesh
	if (name == "XWidth" || name == "YWidth" || name == "ComponentSize" || name == "Height")
	{
		RebuildHeightmap();
		RebuildMesh();
	}
	else if (name == "Material")
	{
		SetMaterial(Material);
	}
}

void ATerrain::BeginPlay()
{
	Super::BeginPlay();

}

void ATerrain::RebuildMesh()
{
	// Destroy the mesh and start over
	Mesh->ClearAllMeshSections();

	for (uint16 x_section = 0; x_section < XWidth; ++x_section)
	{
		for (uint16 y_section = 0; y_section < YWidth; ++y_section)
		{
			// Create vertices
			TArray<FVector> vertices;
			float offset_x = (float)(heightmap_x - 1) / 2.0f - (ComponentSize - 1) * x_section;
			float offset_y = (float)(heightmap_y - 1) / 2.0f - (ComponentSize - 1) * y_section;

			for (uint16 y = 0; y < ComponentSize; ++y)
			{
				for (uint16 x = 0; x < ComponentSize; ++x)
				{
					vertices.Add(FVector(x - offset_x, offset_y - y, ((float)heightmap[(x + x_section * (ComponentSize - 1)) + ((y + y_section * (ComponentSize - 1)) * heightmap_x)] / HMAPMAX - 0.5f) * Height));
				}
			}

			// Create triangles
			TArray<int32> triangles;
			for (uint16 y = 0; y < ComponentSize - 1; ++y)
			{
				for (uint16 x = 0; x < ComponentSize - 1; ++x)
				{
					triangles.Add(x + (y * ComponentSize));
					triangles.Add(1 + x + y * ComponentSize);
					triangles.Add(1 + x + (y + 1) * ComponentSize);

					triangles.Add(x + (y * ComponentSize));
					triangles.Add(1 + x + (y + 1) * ComponentSize);
					triangles.Add(x + (y + 1) * ComponentSize);
				}
			}

			// Create UVs
			TArray<FVector2D> UV0;
			for (uint16 y = 0; y < ComponentSize; ++y)
			{
				for (uint16 x = 0; x < ComponentSize; ++x)
				{
					UV0.Add(FVector2D(x + x_section * (ComponentSize - 1), y + y_section * (ComponentSize - 1)));
				}
			}

			// Create normals
			TArray<FVector> normals;
			for (uint16 y = 0; y < ComponentSize - 1; ++y)
			{
				for (uint16 x = 0; x < ComponentSize - 1; ++x)
				{
					if (x > 0 && x < ComponentSize - 1 && y > 0 && y < ComponentSize - 1)
					{
						normals.Add(FVector(0.0f, 0.0f, 1.0f));
					}
					else
					{
						normals.Add(FVector(0.0f, 0.0f, 1.0f));
					}
				}
			}

			// Create tangents
			TArray<FProcMeshTangent> tangents;

			// Create the mesh
			Mesh->CreateMeshSection(x_section + y_section * XWidth, vertices, triangles, normals, UV0, TArray<FColor>(), tangents, true);
		}
	}

	SetMaterial(Material);
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "Terrain mesh rebuilt");
}

void ATerrain::RebuildHeightmap()
{
	// Determine how much memory to allocate for the heightmap
	heightmap_x = (ComponentSize - 1) * XWidth + 1;
	heightmap_y = (ComponentSize - 1) * YWidth + 1;
	int32 size = heightmap_x * heightmap_y;

	if (size == 0)
	{
		return;
	}

	// Empty the heightmap and reallocate it
	heightmap.Empty(size);
	for (int32 i = 0; i < size; ++i)
	{
		heightmap.Add(HMAPMED);
	}
}

void ATerrain::SetMaterial(UMaterial* material)
{
	Material = material;
	for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
	{
		Mesh->SetMaterial(i, Material);
	}
}

void ATerrain::GenerateSlope(uint16 height)
{
	for (int32 i = 0; i < heightmap_x; ++i)
	{
		for (int32 j = 0; j < heightmap_y; ++j)
		{
			heightmap[i + j * heightmap_x] = HMAPMED + i * height;
		}
	}
}