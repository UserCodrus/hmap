// Copyright © 2019 Created by Brian Faubion

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Terrain.generated.h"

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

	// Rebuild the procedural mesh component
	void RebuildMesh();
	// Create a blank heightmap big enough to accomodate every component
	void RebuildHeightmap();

	// Set the materials for the mesh
	void SetMaterial(UMaterial* material);

	// Create a sloping heightmap
	void GenerateSlope(uint16 height);

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The procedural mesh component used for the terrain mesh
	UPROPERTY(EditAnywhere)
		UProceduralMeshComponent * Mesh = nullptr;

	// The material to apply to each section
	UPROPERTY(EditAnywhere)
		UMaterial* Material = nullptr;

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
	TArray<uint16> heightmap;
	// The dimensions of the heightmap
	uint16 heightmap_x;
	uint16 heightmap_y;
};
