#pragma once

#include "heightmap.h"

// Generate perlin noise on a heightmap
//	frequency =	The scaling factor of the noise
//				Larger values mean smoother maps, smaller values mean bumpier maps
//				Must be at least 1 to avoid noise anomalies
//	octaves =	The number of layers of noise to apply
//				Each layer of noise will have its frequency doubled
//	minimum =	The minimum elevation of the heightmap
//	maximum =	The maximum elevation of the heightmap
void layeredPerlin(Heightmap<uint16_t>& map, unsigned seed, float frequency, unsigned octaves, uint16_t minimum, uint16_t maximum);