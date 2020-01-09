// Copyright © 2019 Created by Brian Faubion

#include "Terrain.h"
#include "Component.h"
#include "Algorithms.h"

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

typedef std::chrono::steady_clock Timer;

ATerrain::ATerrain()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the root component
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	// Create the heightmap
	Map = CreateDefaultSubobject<UHeightMap>(TEXT("HeightMap"));

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
	if (name == "XWidth" || name == "YWidth" || name == "ComponentSize" || name == "Height" || name == "Border" || name == "AutoRebuild")
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
	else if (name == "TerrainMaterial" || name == "BorderMaterial")
	{
		ApplyMaterials();
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
		// Add or remove mesh components until we have one component per terrain section
		uint16 component_count = XWidth * YWidth;
		if (Border)
		{
			++component_count;
		}

		if (Meshes.Num() > component_count)
		{
			// Remove components
			while (Meshes.Num() > component_count)
			{
				Meshes.Last()->DestroyComponent();
				Meshes.Pop();
			}
		}
		else if (Meshes.Num() < component_count)
		{
			// Add components
			uint16 i = Meshes.Num();
			while (Meshes.Num() < component_count)
			{
				// Name the component
				std::string name;
				if (i == (component_count - 1) && Border)
				{
					name = "TerrainBorder";
				}
				else
				{
					name = "TerrainSection" + std::to_string(i);
					++i;
				}

				// Create the component
				Meshes.Add(NewObject<UProceduralMeshComponent>(this, UProceduralMeshComponent::StaticClass(), FName(name.c_str())));
				Meshes.Last()->RegisterComponentWithWorld(GetWorld());
				Meshes.Last()->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			}
		}
	}

#ifdef LOG_TERRAIN_METRICS
	// Get the start time
	auto t_start = Timer::now();
	auto t_current = t_start;

	// Log the time taken for each section
	std::vector<std::chrono::duration<double>> t_sections;
#endif

	// Calculate normals and tangents
	//heightmap.calculateNormals(normalmap, tangentmap, Height);
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	Map->CalculateNormalsAndTangents(Normals, Tangents);

	// Determine the number of threads to use
	uint16 num_threads = 4;
	if (XWidth * YWidth < num_threads)
	{
		num_threads = XWidth * YWidth;
	}

	uint16 section_x = 0;
	uint16 section_y = 0;

	// Start the worker threads and add an inital workload
	TArray<ComponentBuilder> builders;
	for (uint16 i = 0; i < num_threads; ++i)
	{
		if (i > 0)
		{
			// Switch to the next section
			++section_x;
			if (section_x >= XWidth)
			{
				section_x = 0;
				++section_y;
			}
		}

		builders.Emplace(Map, &Normals, &Tangents, Height, ComponentSize);
		builders.Last().Build(section_x, section_y, section_x + section_y * XWidth);
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
					// Add the thread data to a mesh
					if (full_rebuild)
					{
						Meshes[data->section]->CreateMeshSection(0, data->vertices, data->triangles, data->normals, data->UV0, TArray<FColor>(), data->tangents, true);
					}
					else
					{
						Meshes[data->section]->UpdateMeshSection(0, data->vertices, data->normals, data->UV0, TArray<FColor>(), data->tangents);
					}

					// Change sections
					++section_x;
					if (section_x >= XWidth)
					{
						section_x = 0;
						++section_y;
					}

					if (section_y < YWidth)
					{
						// Queue another component
						builders[i].Build(section_x, section_y, section_x + section_y * XWidth);
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

	// Build the border mesh
	if (Border)
	{
		ComponentData edge;
		
		int32 width_x = Map->GetWidthX();
		int32 width_y = Map->GetWidthY();

		float world_x = (float)(Map->GetWidthX() - 1) / 2.0f;
		float world_y = (float)(Map->GetWidthY() - 1) / 2.0f;

		// -X side
		for (uint16 y = 0; y < width_y; ++y)
		{
			edge.vertices.Add(FVector(-world_x, world_y - y, -Height));
			edge.vertices.Add(FVector(-world_x, world_y - y, Map->GetHeight(0, y) * Height));
			edge.normals.Add(FVector(-1.0f, 0.0f, 0.0f));
			edge.normals.Add(FVector(-1.0f, 0.0f, 0.0f));
			edge.tangents.Add(FProcMeshTangent(0.0f, 0.0f, 1.0f));
			edge.tangents.Add(FProcMeshTangent(0.0f, 0.0f, 1.0f));
			edge.UV0.Add(FVector2D(0.0, y));
			edge.UV0.Add(FVector2D((Map->GetHeight(0, y) + 1) * Height, y));
		}

		for (uint16 y = 0; y < width_y - 1; ++y)
		{
			edge.triangles.Add(y * 2);
			edge.triangles.Add(1 + y * 2);
			edge.triangles.Add(1 + (y + 1) * 2);

			edge.triangles.Add(y * 2);
			edge.triangles.Add(1 + (y + 1) * 2);
			edge.triangles.Add((y + 1) * 2);
		}

		uint16 size = edge.vertices.Num();

		// +X side
		for (uint16 y = 0; y < width_y; ++y)
		{
			edge.vertices.Add(FVector(world_x, world_y - y, -Height));
			edge.vertices.Add(FVector(world_x, world_y - y, Map->GetHeight(width_x - 1, y) * Height));
			edge.normals.Add(FVector(1.0f, 0.0f, 0.0f));
			edge.normals.Add(FVector(1.0f, 0.0f, 0.0f));
			edge.tangents.Add(FProcMeshTangent(0.0f, 0.0f, 1.0f));
			edge.tangents.Add(FProcMeshTangent(0.0f, 0.0f, 1.0f));
			edge.UV0.Add(FVector2D(0.0, y));
			edge.UV0.Add(FVector2D((Map->GetHeight(width_x - 1, y) + 1) * Height, y));
		}

		for (uint16 y = 0; y < width_y - 1; ++y)
		{
			edge.triangles.Add(size + 1 + (y + 1) * 2);
			edge.triangles.Add(size + 1 + y * 2);
			edge.triangles.Add(size + y * 2);

			edge.triangles.Add(size + (y + 1) * 2);
			edge.triangles.Add(size + 1 + (y + 1) * 2);
			edge.triangles.Add(size + y * 2);
		}

		size = edge.vertices.Num();

		// +Y side
		for (uint16 x = 0; x < width_x; ++x)
		{
			edge.vertices.Add(FVector(-world_x + x, world_y, -Height));
			edge.vertices.Add(FVector(-world_x + x, world_y, Map->GetHeight(x, 0) * Height));
			edge.normals.Add(FVector(0.0f, 1.0f, 0.0f));
			edge.normals.Add(FVector(0.0f, 1.0f, 0.0f));
			edge.tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
			edge.tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
			edge.UV0.Add(FVector2D(x, 0.0));
			edge.UV0.Add(FVector2D(x, (Map->GetHeight(x, 0) + 1) * Height * 2));
		}

		for (uint16 x = 0; x < width_x - 1; ++x)
		{
			edge.triangles.Add(size + 1 + (x + 1) * 2);
			edge.triangles.Add(size + 1 + x * 2);
			edge.triangles.Add(size + x * 2);

			edge.triangles.Add(size + (x + 1) * 2);
			edge.triangles.Add(size + 1 + (x + 1) * 2);
			edge.triangles.Add(size + x * 2);
		}

		size = edge.vertices.Num();

		// -Y side
		for (uint16 x = 0; x < width_x; ++x)
		{
			edge.vertices.Add(FVector(-world_x + x, -world_y, -Height));
			edge.vertices.Add(FVector(-world_x + x, -world_y, Map->GetHeight(x, width_y - 1) * Height));
			edge.normals.Add(FVector(0.0f, -1.0f, 0.0f));
			edge.normals.Add(FVector(0.0f, -1.0f, 0.0f));
			edge.tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
			edge.tangents.Add(FProcMeshTangent(1.0f, 0.0f, 0.0f));
			edge.UV0.Add(FVector2D(x, 0.0));
			edge.UV0.Add(FVector2D(x, (Map->GetHeight(x, width_y - 1) + 1) * Height));
		}

		for (uint16 x = 0; x < width_x - 1; ++x)
		{
			edge.triangles.Add(size + x * 2);
			edge.triangles.Add(size + 1 + x * 2);
			edge.triangles.Add(size + 1 + (x + 1) * 2);

			edge.triangles.Add(size + x * 2);
			edge.triangles.Add(size + 1 + (x + 1) * 2);
			edge.triangles.Add(size + (x + 1) * 2);
		}

		// Add or update the border component
		if (full_rebuild)
		{
			Meshes.Last()->CreateMeshSection(0, edge.vertices, edge.triangles, edge.normals, edge.UV0, TArray<FColor>(), edge.tangents, true);
		}
		else
		{
			Meshes.Last()->UpdateMeshSection(0, edge.vertices, edge.normals, edge.UV0, TArray<FColor>(), edge.tangents);
		}
	}

	FString output;
	if (full_rebuild)
	{
		ApplyMaterials();
		DirtyMesh = false;

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

	// Resize the heightmap
	Map->Resize(heightmap_x, heightmap_y, Height);

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, "Heightmap rebuilt");
}

void ATerrain::SetMaterials(UMaterial* terrain_material, UMaterial* border_material)
{
	// Change materials
	if (terrain_material != nullptr)
	{
		TerrainMaterial = terrain_material;
	}
	if (border_material != nullptr)
	{
		BorderMaterial = border_material;
	}

	// Set the mesh materials
	ApplyMaterials();
}

void ATerrain::ApplyMaterials()
{
	uint32 mesh_count = Meshes.Num();

	// Set the material for the border mesh
	if (Border)
	{
		--mesh_count;
		Meshes.Last()->SetMaterial(0, BorderMaterial);
	}

	// Set materials for terrain sections
	for (uint32 i = 0; i < mesh_count; ++i)
	{
		Meshes[i]->SetMaterial(0, TerrainMaterial);
	}
}

void ATerrain::GenerateFlat(uint16 height)
{
	// Rebuild the map if needed
	if (DirtyMesh)
	{
		Rebuild();
	}

	for (int32 i = 0; i < Map->GetWidthX(); ++i)
	{
		for (int32 j = 0; j < Map->GetWidthY(); ++j)
		{
			Map->SetHeight(i, j, height);
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

	for (int32 i = 0; i < Map->GetWidthX(); ++i)
	{
		for (int32 j = 0; j < Map->GetWidthY(); ++j)
		{
			Map->SetHeight(i, j, (float)i / (float)Map->GetWidthX() * height);
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

	int32 width_x = Map->GetWidthX();
	int32 width_y = Map->GetWidthY();

	// Create the plasma noise
	PlasmaNoise noise(scale, seed);
	noise.scale(width_x, width_y);

	// Sample the noise onto the terrain
	for (int32 x = 0; x < width_x; ++x)
	{
		for (int32 y = 0; y < width_y; ++y)
		{
			Map->SetHeight(x, y, noise.cubic((float)x, (float)y));
		}
	}

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

	int32 width_x = Map->GetWidthX();
	int32 width_y = Map->GetWidthY();

	// Create noise data
	std::vector<GradientNoise> noise;
	for (unsigned i = 1; i <= octaves; ++i)
	{
		noise.push_back(GradientNoise(frequency * i, frequency * i, seed++));
		noise.back().scale(width_x, width_y);
	}
	
	// Sample the noise onto the terrain
	for (int32 x = 0; x < width_x; ++x)
	{
		for (int32 y = 0; y < width_y; ++y)
		{
			float amplitude = 1.0f;
			float total = 0.0f;
			float height = 0.0f;
			for (unsigned i = 0; i < octaves; ++i)
			{
				height += noise[i].perlin(x, y) * amplitude;
				total += amplitude;
				amplitude *= persistence;
			}

			Map->SetHeight(x, y, height / total);
		}
	}

	RebuildMesh(false);
}