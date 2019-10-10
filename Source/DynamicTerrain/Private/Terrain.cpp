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

	RebuildHeightmap();
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
			RebuildHeightmap();
			RebuildMesh();
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

}

void ATerrain::BuildComponent(ComponentData& data, uint16 component_x, uint16 component_y)
{
	// The number of polygons in each component
	uint16 polygons = ComponentSize - 1;
	uint16 heightmap_x = heightmap.getWidthX();
	uint16 heightmap_y = heightmap.getWidthY();

	// The x and y coordinates of the current component in the world
	float world_x = (float)(heightmap_x - 1) / 2.0f - polygons * component_x;
	float world_y = (float)(heightmap_y - 1) / 2.0f - polygons * component_y;

	// The x and y coordinates of the current component in the heightmap
	uint16 map_x = component_x * polygons;
	uint16 map_y = component_y * polygons;

	// Create vertices
	for (uint16 y = 0; y < ComponentSize; ++y)
	{
		for (uint16 x = 0; x < ComponentSize; ++x)
		{
			data.vertices.Add(FVector(x - world_x, world_y - y, heightmap.getHeight(map_x + x, map_y + y) * Height));
		}
	}

	// Create triangles
	for (uint16 y = 0; y < polygons; ++y)
	{
		for (uint16 x = 0; x < polygons; ++x)
		{
			data.triangles.Add(x + (y * ComponentSize));
			data.triangles.Add(1 + x + y * ComponentSize);
			data.triangles.Add(1 + x + (y + 1) * ComponentSize);

			data.triangles.Add(x + (y * ComponentSize));
			data.triangles.Add(1 + x + (y + 1) * ComponentSize);
			data.triangles.Add(x + (y + 1) * ComponentSize);
		}
	}

	// Create UVs
	for (uint16 y = 0; y < ComponentSize; ++y)
	{
		for (uint16 x = 0; x < ComponentSize; ++x)
		{
			data.UV0.Add(FVector2D(x + map_x, y + map_y));
		}
	}

	// Create normals and tangents
	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(data.vertices, data.triangles, data.UV0, data.normals, data.tangents);
}

void ATerrain::RebuildMesh()
{
	// Destroy the mesh and start over
	Mesh->ClearAllMeshSections();

#ifdef LOG_TERRAIN_METRICS
	// Get the start time
	auto t_start = Timer::now();
	auto t_current = t_start;

	// Log the time taken for each section
	std::vector<std::chrono::duration<double>> t_sections;
#endif

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

		builders.Emplace(&heightmap, Height, ComponentSize);
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
					Mesh->CreateMeshSection(data->section, data->vertices, data->triangles, data->normals, data->UV0, TArray<FColor>(), data->tangents, true);

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
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "Terrain mesh rebuilt");

#ifdef LOG_TERRAIN_METRICS
	std::string tmp;
	std::stringstream log;
	FString output;

	// Log the total time
	std::chrono::duration<double> total = Timer::now() - t_start;

	log << "Total duration: " << total.count() << "s";
	tmp = log.str();
	output = tmp.c_str();

	GEngine->AddOnScreenDebugMessage(-1, 9.0f, FColor::Yellow, output);
	log.str("");

	// Log the average time
	/*double average = 0.0;
	for (size_t i = 0; i < t_sections.size(); ++i)
	{
		average += t_sections[i].count();
	}
	average /= t_sections.size();

	log << "Average section time: " << average << "s";
	tmp = log.str();
	output = tmp.c_str();

	GEngine->AddOnScreenDebugMessage(-1, 9.0f, FColor::Yellow, output);*/
#endif
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
	for (unsigned i = 0; i < heightmap.getWidthX(); ++i)
	{
		for (unsigned j = 0; j < heightmap.getWidthY(); ++j)
		{
			heightmap.setHeight(i, j, 0.0f);
		}
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

void ATerrain::GenerateFlat(uint16 height)
{
	for (unsigned i = 0; i < heightmap.getWidthX(); ++i)
	{
		for (unsigned j = 0; j < heightmap.getWidthY(); ++j)
		{
			heightmap.setHeight(i, j, height);
		}
	}
}

void ATerrain::GenerateSlope(float height)
{
	for (unsigned i = 0; i < heightmap.getWidthX(); ++i)
	{
		for (unsigned j = 0; j < heightmap.getWidthY(); ++j)
		{
			heightmap.setHeight(i, j, (float)i / (float)heightmap.getWidthX() * height);
		}
	}
}

void ATerrain::GeneratePlasma(unsigned scale)
{
	unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
	MapGenerator::plasma(heightmap, seed, -1.0f, 1.0f, scale);
}

void ATerrain::GeneratePerlin(unsigned frequency, unsigned octaves, float persistence)
{
	unsigned seed = (unsigned)std::chrono::steady_clock::now().time_since_epoch().count();
	MapGenerator::layeredPerlin(heightmap, seed, -1.0, 1.0f, frequency, octaves, persistence);
}