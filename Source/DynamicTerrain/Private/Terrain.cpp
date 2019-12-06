// Copyright © 2019 Created by Brian Faubion

#include "Terrain.h"
#include "Component.h"
#include "generate.h"

#include <chrono>

#ifdef LOG_TERRAIN_METRICS
#include <string>
#include <sstream>
#include <vector>
#endif

#include "Engine/Engine.h"
#include "KismetProceduralMeshLibrary.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/Material.h"

// The largest value stored in the heightmap
#define HMAPMAX 65535
// The median value of the heightmap
#define HMAPMED 32767

typedef std::chrono::steady_clock Timer;

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
}

void ATerrain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATerrain::PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FString name = PropertyChangedEvent.MemberProperty->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "Property Changed: " + name);

	// If the mesh properties change, rebuild the heightmap and mesh
	if (name == "XWidth" || name == "YWidth" || name == "ComponentSize" || name == "Height" || name == "AutoRebuild")
	{
		if (AutoRebuild)
		{
			// Rebuild the mesh if autorebuild is enabled
			Rebuild();
		}
		else
		{
			// Mark the mesh as dirty
			if (name != "AutoRebuild")
			{
				DirtyMesh = true;
			}
		}
	}
	else if (name == "Material")
	{
		SetMaterial(Material);
	}
}

void ATerrain::BeginPlay()
{
	Super::BeginPlay();

	RebuildHeightmap();
}

void ATerrain::Rebuild()
{
	RebuildHeightmap();
	RebuildMesh(true);
}

void ATerrain::RebuildMesh(bool full_rebuild)
{
	// Destroy the mesh and start over
	if (full_rebuild)
	{
		Mesh->ClearAllMeshSections();
	}

#ifdef LOG_TERRAIN_METRICS
	// Get the start time
	auto t_start = Timer::now();
	auto t_current = t_start;

	// Log the time taken for each section
	std::vector<std::chrono::duration<double>> t_sections;
#endif

	// Calculate normals and tangents
	heightmap.calculateNormals(normalmap, tangentmap, Height);

	// Determine the number of threads to use
	uint16 num_threads = 4;
	if (XWidth * YWidth < num_threads)
	{
		num_threads = XWidth * YWidth;
	}

	uint16 x = 0;
	uint16 y = 0;

	// Start the worker threads and add an inital workload
	TArray<ComponentBuilder> builders;
	for (uint16 i = 0; i < num_threads; ++i)
	{
		if (i > 0)
		{
			// Switch to the next section
			++x;
			if (x >= XWidth)
			{
				x = 0;
				++y;
			}
		}

		builders.Emplace(&heightmap, &normalmap, &tangentmap, Height, ComponentSize);
		builders.Last().Build(x, y, x + y * XWidth);
	}

	// Run until all threads are closed
	while (builders.Num() > 0)
	{
		for (uint16 i = 0; i < builders.Num(); ++i)
		{
			// Check to see if the thread is idling
			if (builders[i].IsIdle())
			{
				ComponentData* data = builders[i].GetData();
				if (data != nullptr)
				{
					// Add the thread data to the mesh
					if (full_rebuild)
					{
						Mesh->CreateMeshSection(data->section, data->vertices, data->triangles, data->normals, data->UV0, TArray<FColor>(), data->tangents, true);
					}
					else
					{
						Mesh->UpdateMeshSection(data->section, data->vertices, data->normals, data->UV0, TArray<FColor>(), data->tangents);
					}

					// Change sections
					++x;
					if (x >= XWidth)
					{
						x = 0;
						++y;
					}

					if (y < YWidth)
					{
						// Queue another component
						builders[i].Build(x, y, x + y * XWidth);
					}
					else
					{
						// Close the thread if all components have been built
						builders.RemoveAt(i);
						break;
					}
				}
			}
		}

		// Sleep between checking threads to avoid eating resources
		FPlatformProcess::Sleep(0.01);
	}

	SetMaterial(Material);
	DirtyMesh = false;

	FString output;
	if (full_rebuild)
	{
		output = "Terrain mesh rebuilt";
	}
	else
	{
		output = "Terrain mesh updated";
	}

#ifdef LOG_TERRAIN_METRICS
	std::string tmp;
	std::stringstream log;

	// Log the total time
	std::chrono::duration<double> total = Timer::now() - t_start;

	log << " in  " << total.count() << "s";
	tmp = log.str();
	output += tmp.c_str();
#endif

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, output);
}

void ATerrain::RebuildHeightmap()
{
	// Determine how much memory to allocate for the heightmap
	uint16 heightmap_x = (ComponentSize - 1) * XWidth + 1;
	uint16 heightmap_y = (ComponentSize - 1) * YWidth + 1;

	if (heightmap_x * heightmap_y == 0)
	{
		return;
	}

	// Empty the heightmap and reallocate it
	heightmap.resize(heightmap_x, heightmap_y);
	normalmap.resize(heightmap_x, heightmap_y);
	tangentmap.resize(heightmap_x, heightmap_y);

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, "Heightmap rebuilt");
}

void ATerrain::SetMaterial(UMaterial* material)
{
	Material = material;
	for (int32 i = 0; i < Mesh->GetNumMaterials(); ++i)
	{
		Mesh->SetMaterial(i, Material);
	}
}

void ATerrain::GenerateFlat(uint16 height)
{
	// Rebuild the map if needed
	if (DirtyMesh)
	{
		Rebuild();
	}

	for (unsigned i = 0; i < heightmap.getWidthX(); ++i)
	{
		for (unsigned j = 0; j < heightmap.getWidthY(); ++j)
		{
			heightmap.setHeight(i, j, height);
		}
	}

	RebuildMesh(false);
}

void ATerrain::GenerateSlope(float height)
{
	// Rebuild the map if needed
	if (DirtyMesh)
	{
		Rebuild();
	}

	for (unsigned i = 0; i < heightmap.getWidthX(); ++i)
	{
		for (unsigned j = 0; j < heightmap.getWidthY(); ++j)
		{
			heightmap.setHeight(i, j, (float)i / (float)heightmap.getWidthX() * height);
		}
	}

	RebuildMesh(false);
}

void ATerrain::GeneratePlasma(unsigned scale)
{
	// Rebuild the map if needed
	if (DirtyMesh)
	{
		Rebuild();
	}

	unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
	MapGenerator::plasma(heightmap, seed, -1.0f, 1.0f, scale);

	RebuildMesh(false);
}

void ATerrain::GeneratePerlin(unsigned frequency, unsigned octaves, float persistence)
{
	// Rebuild the map if needed
	if (DirtyMesh)
	{
		Rebuild();
	}

	unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
	MapGenerator::layeredPerlin(heightmap, seed, -1.0, 1.0f, frequency, octaves, persistence);

	RebuildMesh(false);
}