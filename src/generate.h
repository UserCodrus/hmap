#pragma once

#include "heightmap.h"

/*
 * Default parameters for all map generators:
 *
 * map:		The heightmap that data will be written to - any existing height data will be overwritten
 * seed:	The RNG seed used to generate map data
 * min:		The minimum elevation of the heightmap produced
 * max:		The maximum elevation of the heightmap produced
 */
namespace MapGenerator
{
	// The default heightmap generator
	void defaultGenerator(Heightmap& map, unsigned seed, float min, float max);

	/*
	 * Generate a heightmap using the diamond-square fractal pattern
	 *
	 * scale:		The scale of the noise map generated - the frequency of the noise will be equal to (2 ^ scale) + 1
	 */
	void plasma(Heightmap& map, unsigned seed, float min, float max, unsigned scale);

	/*
	 * Generate a heightmap using multiple layers of white noise stacked on top of one another, with the frequency of each layer doubling
	 *
	 * frequency:	The frequency of the first layer of noise - lower frequency mean smoother noise, while higher frequency will be rougher and bumpier (must be > 2)
	 * octaves:		The number of layers of noise to use - lower numbers of ocataves results in smoother and simpler noise, higher octaves are more diverse and rough (must be > 0)
	 * persistence:	The level of influence each successive octave has - higher persistence results in bumpier terrain, while lower persistence creates smoother terrain (must be between 0.0 and 1.0)
	 */
	void layeredWhiteNoise(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence);

	/*
	 * Generate a heightmap using multiple layers of Perlin noise stacked on top of one another, with the frequency of each layer doubling
	 *
	 * frequency:	The frequency of the first layer of noise - lower frequency mean smoother noise, while higher frequency will be rougher and bumpier (must be > 2)
	 * octaves:		The number of layers of noise to use - lower numbers of ocataves results in smoother and simpler noise, higher octaves are more diverse and rough (must be > 0)
	 * persistence:	The level of influence each successive octave has - higher persistence results in bumpier terrain, while lower persistence creates smoother terrain (must be between 0.0 and 1.0)
	 */
	void layeredPerlin(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence);
}