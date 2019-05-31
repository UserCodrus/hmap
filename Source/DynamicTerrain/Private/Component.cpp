#include "Component.h"

ComponentData::ComponentData(unsigned size)
{
	vertices.Reserve(size * size);
	triangles.Reserve((size - 1) * (size - 1) * 6);
	UV0.Reserve(size * size);
	normals.Reserve(size * size);
	tangents.Reserve(size * size);
}