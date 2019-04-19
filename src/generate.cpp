#include "generate.h"
#include "algorithm.h"

void perlinOctaves(Heightmap<uint16_t>& map, uint16_t min, uint16_t max, unsigned seed, unsigned grid_size, unsigned octaves, float persistence)
{
	// Create the gradient grid
	GradientGrid grad(grid_size, grid_size, seed);

	// Apply noise
	for (unsigned y = 0; y < map.getWidthY(); ++y)
	{
		for (unsigned x = 0; x < map.getWidthX(); ++x)
		{
			// Generate multiple layers of noise
			float noise = 0.0f;
			float amplitude = 1.0f;
			float total_amplitude = 0.0f;

			// Set the frequency so that the map coordinates will never be outside the grid
			float fx = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / (map.getWidthX() + 2));
			float fy = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / (map.getWidthY() + 2));
			
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

void octavePerlinF(Heightmap<uint16_t>& map, unsigned seed)
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