#pragma once

#include "heightmap.h"

// The default heightmap generator
void defaultGenerator(Heightmap& map, unsigned seed, float min, float max);

/*
 * Generate a heightmap using multiple layers of random noise stacked on top of one another, with the frequency of each layer doubling
 *
 * seed:		The rng seed to use for noise generation
 * min:			The minimum elevation of the heightmap produced
 * max:			The maximum elevation of the heightmap produced
 *
 * frequency:	The frequency of the first layer of noise - lower frequency mean smoother noise, while higher frequency will be rougher and bumpier (must be > 2)
 * octaves:		The number of layers of noise to use - lower numbers of ocataves results in smoother and simpler noise, higher octaves are more diverse and rough (must be > 0)
 * persistence:	The level of influence each successive octave has - higher persistence results in bumpier terrain, while lower persistence creates smoother terrain (must be between 0.0 and 1.0)
 */
void randomGenerator(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence);

/*
 * Generate a heightmap using the diamond-square fractal pattern
 *
 * seed:		The rng seed to use for noise generation
 * min:			The minimum elevation of the heightmap produced
 * max:			The maximum elevation of the heightmap produced
 *
 * scale:		The scale of the  noise map generated - the frequency of the noise will be equal to (2 ^ scale) + 1
 */
void plasmaGenerator(Heightmap& map, unsigned seed, float min, float max, unsigned scale);

/*
 * Generate a heightmap using multiple layers of Perlin noise stacked on top of one another, with the frequency of each layer doubling
 *
 * seed:		The rng seed to use for noise generation
 * min:			The minimum elevation of the heightmap produced
 * max:			The maximum elevation of the heightmap produced
 *
 * frequency:	The frequency of the first layer of noise - lower frequency mean smoother noise, while higher frequency will be rougher and bumpier (must be > 2)
 * octaves:		The number of layers of noise to use - lower numbers of ocataves results in smoother and simpler noise, higher octaves are more diverse and rough (must be > 0)
 * persistence:	The level of influence each successive octave has - higher persistence results in bumpier terrain, while lower persistence creates smoother terrain (must be between 0.0 and 1.0)
 */
void perlinGenerator(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence);