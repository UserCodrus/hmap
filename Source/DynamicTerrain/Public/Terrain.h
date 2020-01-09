// Copyright © 2019 Created by Brian Faubion

#pragma once

#include "HeightMap.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Terrain.generated.h"

struct ComponentData;

UCLASS()
class DYNAMICTERRAIN_API ATerrain : public AActor
{
	GENERATED_BODY()
	
public:	

	// Sets default values for this actor's properties
	ATerrain();

	/// Engine Functions ///

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;

	/// Basic Functions ///

	// Rebuild the terrain
	void Rebuild();

	// Set the materials for the mesh
	void SetMaterials(UMaterial* terrain_material = nullptr, UMaterial* border_material = nullptr);

	/// Generator Functions ///

	// Flatten heightmap
	void GenerateFlat(uint16 height);
	// Create a sloping heightmap
	void GenerateSlope(float height);
	// Create a heightmap from plasma noise
	void GeneratePlasma(unsigned scale);
	// Create a heightmap from perlin noise
	void GeneratePerlin(unsigned frequency, unsigned octaves, float persistence);

protected:

	/// Engine Functions ///

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// Private Functions ///

	// Rebuild or reload the procedural mesh component
	void RebuildMesh(bool full_rebuild);
	// Create a blank heightmap big enough to accomodate every component
	void RebuildHeightmap();

	// Refresh the materials on component meshes
	void ApplyMaterials();

	/// Properties ///

	// The procedural mesh component used for the terrain mesh
	UPROPERTY(EditAnywhere)
		TArray<UProceduralMeshComponent*> Meshes;

	// The material to apply to the terrain
	UPROPERTY(EditAnywhere)
		UMaterial* TerrainMaterial = nullptr;
	// The material for the outer border
	UPROPERTY(EditAnywhere)
		UMaterial* BorderMaterial = nullptr;

	// If set to true the terrain will automatically rebuild itself when altered in the editor
	UPROPERTY(EditAnywhere)
		bool AutoRebuild = false;

	// Set to true to create borders on the map mesh
	UPROPERTY(EditAnywhere)
		bool Border = true;

	// The number of vertices per component section
	UPROPERTY(EditAnywhere)
		uint16 ComponentSize = 64;
	// The number of component sections in the mesh
	UPROPERTY(EditAnywhere)
		uint16 XWidth = 1;
	UPROPERTY(EditAnywhere)
		uint16 YWidth = 1;
	// The maximum height of the heightmap measured from the lowest point to the highest
	UPROPERTY(EditAnywhere)
		float Height = 512.0f;

	// Set to true when the map mesh needs to be rebuilt
	UPROPERTY(VisibleAnywhere)
		bool DirtyMesh = true;

	// The heightmap used to store terrain data
	UPROPERTY(VisibleAnywhere)
		UHeightMap* Map;

	// The heightmap used to store data
	//Heightmap heightmap;
	// The normals and tangents for the heightmap
	//Vectormap normalmap;
	//Vectormap tangentmap;
};
