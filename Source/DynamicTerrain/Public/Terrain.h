// Copyright © 2019 Created by Brian Faubion

#pragma once

#include "heightmap.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Terrain.generated.h"

struct ComponentData;

UCLASS()
class UHeightMap : public UObject
{
	GENERATED_BODY()

public:
	/// Blueprint Functions ///

	// Resize the heightmap
	// X, Y = The width of the heightmap
	// Z = The maximum height of the heightmap
	UFUNCTION(BlueprintCallable)
		void Resize(int32 X, int32 Y, int32 Z);

	// Get the value of the heightmap at the specified coordinates
	UFUNCTION(BlueprintCallable)
		float BPGetHeight(int32 X, int32 Y) const;

	// Calculate the normals and tangents of the map mesh
	UFUNCTION(BlueprintCallable)
		void CalculateNormalsAndTangents(TArray<FVector>& Normals, TArray<FProcMeshTangent>& Tangents) const;

	/// Native Functions ///

	inline float GetHeight(uint32 X, uint32 Y) const;
	inline void SetHeight(uint32 X, uint32 Y, float Height);

	inline int32 GetWidthX() const;
	inline int32 GetWidthY() const;

protected:
	// The height data for the map
	UPROPERTY(VisibleAnywhere)
		TArray<float> MapData;

	// The dimensions of the heightmap
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 WidthX = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 WidthY = 0;

	// The maximum height of the heightmap
	UPROPERTY(VisibleAnywhere)
		float MaxHeight = 1.0f;
};

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
