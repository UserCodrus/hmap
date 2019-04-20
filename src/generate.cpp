#include "generate.h"
#include "algorithm.h"

void perlinOctaves(Heightmap<uint16_t>& map, uint16_t min, uint16_t max, unsigned seed, unsigned grid_size, unsigned octaves, float persistence)
{
	// Create the gradient grid
	GradientGrid grad(grid_size, grid_size, seed);
	unsigned width = map.getWidthX();
	unsigned height = map.getWidthY();

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

	// Apply noise
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			// Generate multiple layers of noise
			float noise = 0.0f;
			float amplitude = 1.0f;
			float total_amplitude = 0.0f;

			// Set the frequency so that the map coordinates will never be outside the grid
			float fx = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / (width + 2));
			float fy = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / (height + 2));
			
			for (unsigned o = 0; o < octaves; ++o)
			{
				noise += perlin(grad, (x + 1) * fx, (y + 1) * fy) * amplitude;
				total_amplitude += amplitude;
				amplitude *= persistence;
				fx *= 2;
				fy *= 2;
			}

			noise = (noise / total_amplitude + 1.0f) * 0.5f;
			map.setHeight(x, y, (uint16_t)(noise * (max - min)) + min);
		}
	}
}

void perlinNotch(Heightmap<uint16_t>& map, uint16_t min, uint16_t max, unsigned seed, unsigned grid_size, float roughness, float detail)
{
	// Create the gradient grid
	GradientGrid grad1(grid_size, grid_size, seed);
	GradientGrid grad2(grid_size, grid_size, seed);
	GradientGrid grad3(grid_size, grid_size, seed);
	unsigned width = map.getWidthX();
	unsigned height = map.getWidthY();

	// Check input values
	if (grid_size < 2)
	{
		grid_size = 2;
	}
	if (detail < 0.0f)
	{
		detail = 0.0f;
	}
	else if (detail > 1.0f)
	{
		detail = 1.0f;
	}
	if (roughness < 0.0f)
	{
		roughness = 0.0f;
	}
	else if (roughness > 1.0f)
	{
		roughness = 1.0f;
	}

	// Set the frequency of the heightmaps to avoid going outside of the gradient grid
	float fx = (float)(grid_size - 1) / (width + 2);
	float fy = (float)(grid_size - 1) / (height + 2);

	// Apply noise
	float elevation, rough, details;
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			elevation = perlin(grad1, (x + 1) * roughness * fx, (y + 1) * roughness * fy);
			rough = perlin(grad2, (x + 1) * roughness * fx, (y + 1) * roughness * fy);
			details = perlin(grad3, (x + 1) * fx, (y + 1) * fy);

			map.setHeight(x, y, (uint16_t)(((elevation + rough * details * detail) / 4.0f + 0.5f) * (max - min)) + min);
		}
	}
}

void testperlin(Heightmap<uint16_t>& map, unsigned seed)
{
	// Create the gradient grid
	std::vector<int> p;
	permutation(p, 256, seed);

	// Apply noise
	for (unsigned y = 0; y < map.getWidthY(); ++y)
	{
		for (unsigned x = 0; x < map.getWidthX(); ++x)
		{
			float noise = (perlin(p.data(), x / 64.0f + 1, y / 64.0f + 1) + 1.0f) * 0.5f;
			map.setHeight(x, y, (uint16_t)(noise * 0xFFFF));
		}
	}
}