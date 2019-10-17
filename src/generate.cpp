#include "generate.h"
#include "algorithm.h"

#include <iostream>

void MapGenerator::defaultGenerator(Heightmap& map, unsigned seed, float min, float max)
{
	PointNoise noise(5, 5, 100, seed);
	map.sample(noise, &PointNoise::worley);

	//GradientNoise base(10, 10, seed);
	//map.sample(base, &GradientNoise::perlin);

	//ValueNoise noise(10, 10, seed);
	//map.sample(noise, &ValueNoise::cubic);

	//GridNoise noise(10, 10, seed);
	//map.sample(noise, &GridNoise::worley);
}

void MapGenerator::plasma(Heightmap& map, unsigned seed, float min, float max, unsigned scale)
{
	// Check input values
	if (scale < 2)
	{
		scale = 2;
	}

	// Generate noise
	PlasmaNoise noise(scale, seed);
	noise.scale(map.getWidthX(), map.getWidthY());

	// Apply noise
	map.sample<PlasmaNoise>(noise, &PlasmaNoise::cubic);

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Scale the noise to fit within the specified limits
	map.multiply(delta);
	map.add(bottom);
}

void MapGenerator::layeredWhiteNoise(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence)
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

	// Create noise data
	std::vector<ValueNoise> noise;
	unsigned width_x = map.getWidthX();
	unsigned width_y = map.getWidthY();
	for (unsigned i = 1; i <= octaves; ++i)
	{
		noise.push_back(ValueNoise(frequency * i, frequency * i, seed++));
		noise.back().scale(width_x, width_y);
	}

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Sample each octave of noise into the heightmap
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			float amplitude = 1.0f;
			float total_amplitude = 0.0f;
			float height = 0.0f;
			for (unsigned i = 0; i < octaves; ++i)
			{
				height += noise[i].cubic((float)x, (float)y) * amplitude;
				total_amplitude += amplitude;
				amplitude *= persistence;
			}

			map.setHeight(x, y, height);
		}
	}
}

void MapGenerator::layeredPerlin(Heightmap& map, unsigned seed, float min, float max, unsigned frequency, unsigned octaves, float persistence)
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

	// Create noise data
	std::vector<GradientNoise> noise;
	unsigned width_x = map.getWidthX();
	unsigned width_y = map.getWidthY();
	for (unsigned i = 1; i <= octaves; ++i)
	{
		noise.push_back(GradientNoise(frequency * i, frequency * i, seed++));
		noise.back().scale(width_x, width_y);
	}

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Sample each octave of noise into the heightmap
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			float amplitude = 1.0f;
			float total_amplitude = 0.0f;
			float height = 0.0f;
			for (unsigned i = 0; i < octaves; ++i)
			{
				height += noise[i].perlin((float)x, (float)y) * amplitude;
				total_amplitude += amplitude;
				amplitude *= persistence;
			}

			map.setHeight(x, y, height);
		}
	}
}