#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "HAL/Runnable.h"

#include "heightmap.h"

struct ComponentData
{
	void allocate(uint16 size, uint16 section_id);

	uint16 section;
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
};

class ComponentBuilder : public FRunnable
{
public:
	ComponentBuilder(Heightmap* map, Vectormap* normal, Vectormap* tangent, float map_height, uint16 component_size);
	~ComponentBuilder();

	// FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	// Builder interface
	bool IsIdle();
	void Build(uint16 _component_x, uint16 _component_y, uint16 section_number);
	ComponentData* GetData();

	FRunnableThread* thread = nullptr;		// The thread this interface runs on
	FThreadSafeCounter counter;				// Thread counter for managing the thread
	ComponentData data;						// The container for the data

private:
	Heightmap* heightmap;					// The heightmap to draw data from
	Vectormap* normalmap;					// The vectormap to draw normal data from
	Vectormap* tangentmap;					// The vectormap to draw tangent data from

	float total_height;						// The Heigth of the parent terrain
	uint16 component_width;					// The ComponentSize of the parent terrain

	uint16 component_x;
	uint16 component_y;

	bool idle;
};