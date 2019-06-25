#include "heightmap.h"
#include "algorithm.h"

void Heightmap::perlinOctaves(unsigned seed, float min, float max, unsigned grid_size, unsigned octaves, float persistence)
{
	// Check input values
	if (grid_size < 2)
	{
		grid_size = 2;
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

	// Create the gradient grid
	GradientNoise grad(grid_size, grid_size, seed);

	// Get the limits of the heightmap
	float delta = max - min;

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			// Generate multiple layers of noise
			float noise = 0.0f;
			float amplitude = 1.0f;
			float total_amplitude = 0.0f;

			// Set the frequency so that the map coordinates will never be outside the grid
			float fx = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / width_x);
			float fy = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / width_y);
			
			for (unsigned o = 0; o < octaves; ++o)
			{
				noise += grad.perlin(x * fx, y * fy) * amplitude;
				total_amplitude += amplitude;
				amplitude *= persistence;
				fx *= 2;
				fy *= 2;
			}

			noise = noise / total_amplitude;
			setHeight(x, y, noise);
		}
	}
}

void Heightmap::perlinNotch(unsigned seed, float min, float max, unsigned base_frequency, unsigned detail_frequency, float detail_level)
{
	// Check input values
	if (base_frequency < 1)
	{
		base_frequency = 1;
	}
	if (detail_frequency < 1)
	{
		detail_frequency = 1;
	}
	if (detail_level < 0.0f)
	{
		detail_level = 0.0f;
	}
	else if (detail_level > 1.0f)
	{
		detail_level = 1.0f;
	}
	
	// Create the gradient grid
	GradientNoise base(base_frequency + 1, base_frequency + 1, seed);
	GradientNoise mod(detail_frequency + 1, detail_frequency + 1, ++seed);

	// Reduce the frequency of the noise a little so heightmap coordinates stay inside the grid
	float fx_base = (float)(base.getWidth() - 1) / width_x;
	float fy_base = (float)(base.getHeight() - 1) / width_y;
	float fx_mod = (float)(mod.getWidth() - 1) / width_x;
	float fy_mod = (float)(mod.getHeight() - 1) / width_y;

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Apply noise
	float elevation, detail;
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			elevation = base.perlin(x * fx_base, y * fy_base);
			detail = mod.perlin(x * fx_mod, y * fy_mod);

			setHeight(x, y, (elevation + elevation * detail * detail_level) * delta + bottom);
		}
	}
}

void Heightmap::random(unsigned seed, float min, float max, unsigned frequency)
{
	// Generate noise
	ValueNoise grid(frequency, frequency, seed);

	// Add frequency constants so the heightmap coordinates stay in the grid
	float fx = (float)(grid.getWidth() - 1) / width_x;
	float fy = (float)(grid.getHeight() - 1) / width_y;

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			setHeight(x, y, grid.cubic(x * fx, y * fy) * delta + bottom);
		}
	}
}

void Heightmap::diamondSquare(unsigned seed, float min, float max, unsigned size)
{
	// Generate noise
	PlasmaNoise grid(size, seed);

	// Add frequency constants so the heightmap coordinates stay in the grid
	float fx = (float)(grid.getWidth() - 1) / width_x;
	float fy = (float)(grid.getHeight() - 1) / width_y;

	// Get the limits of the heightmap
	float delta = (max - min) / 2.0f;
	float bottom = min + delta;

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			setHeight(x, y, grid.cubic(x * fx, y * fy) * delta + bottom);
		}
	}
}

void Heightmap::def(unsigned seed, float min, float max)
{
	//perlinOctaves( seed, min, max, 16, 1, 1.0f);
	//perlinNotch(seed, min, max, 3, 12, 0.6f);
	//random(seed, min, max, 16);
	//diamondSquare(seed, min, max, 8);

	// Create the gradient grid
	GradientNoise base(8, 8, seed);

	// Apply the noise
	noiseAdd<GradientNoise>(base, &GradientNoise::perlin);
}