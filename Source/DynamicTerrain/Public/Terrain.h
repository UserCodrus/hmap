// Copyright © 2019 Created by Brian Faubion

#pragma once

#include "heightmap.h"

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

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent & PropertyChangedEvent) override;

	// Create mesh component data
	void BuildComponent(ComponentData& data, uint16 component_x, uint16 component_y);
	// Rebuild the procedural mesh component
	void RebuildMesh();
	// Create a blank heightmap big enough to accomodate every component
	void RebuildHeightmap();

	// Set the materials for the mesh
	void SetMaterial(UMaterial* material);

	// Flatten heightmap
	void GenerateFlat(uint16 height);
	// Create a sloping heightmap
	void GenerateSlope(float height);
	// Create a heightmap from plasma noise
	void GeneratePlasma(unsigned scale);
	// Create a heightmap from perlin noise
	void GeneratePerlin(unsigned frequency, unsigned octaves, float persistence);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The procedural mesh component used for the terrain mesh
	UPROPERTY(EditAnywhere)
		UProceduralMeshComponent * Mesh = nullptr;

	// The material to apply to each section
	UPROPERTY(EditAnywhere)
		UMaterial* Material = nullptr;

	// If set to true the terrain will automatically rebuild itself when altered in the editor
	UPROPERTY(EditAnywhere)
		bool AutoRebuild = false;

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

	// The heightmap used to store data
	Heightmap heightmap;
};
