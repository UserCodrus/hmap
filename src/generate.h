#pragma once

#include "heightmap.h"

/*
 * Generate a heightmap using multiple layers of Perlin noise stacked on top of one another
 * 
 * min:			The minimum elevation of the heightmap produced
 * max:			The maximum elevation of the heightmap produced
 * seed:		The rng seed to use for noise generation
 * grid_size:	The size of the gradient grid used for noise generation - smaller grids mean smoother noise, while larger grids will be granier with steep elevation changes (must be > 2)
 * octaves:		The number of layers of noise to use - lower octaves make heightmaps very rough and bumpy, while higher octaves are softer and more homogeneous (must be > 0)
 * persistence:	The level of influence each successive octave has - higher persistence results in bumpier terrain, while lower persistence creates smooth hills (must be between 0.0 and 1.0)
 * 
 */
void perlinOctaves(Heightmap<uint16_t>& map, uint16_t min, uint16_t max, unsigned seed, unsigned grid_size, unsigned octaves, float persistence);

/*
 * Generate a heightmap using three layers of Perlin noise
 *
 * min:					The minimum elevation of the heightmap produced
 * max:					The maximum elevation of the heightmap produced
 * seed:				The rng seed to use for noise generation
 * base_frequency:		The frequency of the base noise - smaller frequency mean smoother terrain, while higher frequency will have steep hills (must be > 0)
 * detail_frequency:	Affects the roughness of the terrain - low roughness produces smoother hills, and high roughness produces more bumpy, mountainous terrain (must be > 0)
 * detail_level:		Affects the strength of the roughness - low detail means smooth, tall hills, while higher details means bumpy terrain (must be between 0.0 and 1.0)
 *
 */
void perlinNotch(Heightmap<uint16_t>& map, uint16_t min, uint16_t max, unsigned seed, unsigned base_frequency, unsigned detail_frequency, float detail_level);