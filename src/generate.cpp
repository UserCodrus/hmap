#include "generate.h"
#include "algorithm.h"

#include <iostream>

void defaultGenerator(Heightmap& map, unsigned seed, float min, float max)
{
	PointNoise noise(5, 5, 100, seed);
	map.set(noise, &PointNoise::worley);

	//GradientNoise base(10, 10, seed);
	//map.set(base, &GradientNoise::perlin);

	//ValueNoise noise(10, 10, seed);
	//map.set(noise, &ValueNoise::cubic);

	//GridNoise noise(10, 10, seed);
	//map.set(noise, &GridNoise::worley);
}

void randomGenerator(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence)
{
	// Check input values
	if (frequency < 2)
	{
		frequency = 2;
	}
	if (octaves < 1)
	{
		octaves = 1;
	}
	if (persistence < 0.0f)
	{
		persistence = 0.0f;
	}
	else if (persistence > 1.0f)
	{
		persistence = 1.0f;
	}

	float amplitude = 1.0f;
	float total_amplitude = 0.0f;
	for (unsigned i = 1; i <= octaves; ++i)
	{
		// Create the noise
		ValueNoise noise(frequency * i, frequency * i, seed);

		// Sample the noise to the heightmap
		map.add(noise, &ValueNoise::cubic, amplitude);

		// Adjust the amplitude and the seed
		total_amplitude += amplitude;
		amplitude *= persistence;
		++seed;
	}

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Adjust the map to normalize the amplitude and fit within the specified limits
	map.multiply(delta / total_amplitude);
	map.add(bottom);
}

void plasmaGenerator(Heightmap& map, unsigned seed, float min, float max, unsigned scale)
{
	// Generate noise
	PlasmaNoise noise(scale, seed);

	// Apply noise
	map.set<PlasmaNoise>(noise, &PlasmaNoise::cubic);

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Scale the noise to fit within the specified limits
	map.multiply(delta);
	map.add(bottom);
}

void perlinGenerator(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence)
{
	// Check input values
	if (frequency < 2)
	{
		frequency = 2;
	}
	if (octaves < 1)
	{
		octaves = 1;
	}
	if (persistence < 0.0f)
	{
		persistence = 0.0f;
	}
	else if (persistence > 1.0f)
	{
		persistence = 1.0f;
	}

	float amplitude = 1.0f;
	float total_amplitude = 0.0f;
	for (unsigned i = 1; i <= octaves; ++i)
	{
		// Create the noise
		GradientNoise noise(frequency * i, frequency * i, seed);

		// Sample the noise to the heightmap
		map.add(noise, &GradientNoise::perlin, amplitude);
		
		// Adjust the amplitude and the seed
		total_amplitude += amplitude;
		amplitude *= persistence;
		++seed;
	}

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Adjust the map to normalize the amplitude and fit within the specified limits
	map.multiply(delta / total_amplitude);
	map.add(bottom);
}