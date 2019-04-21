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

void perlinNotch(Heightmap<uint16_t>& map, uint16_t min, uint16_t max, unsigned seed, unsigned base_frequency, unsigned detail_frequency, float detail_level)
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
	GradientGrid base(base_frequency + 1, base_frequency + 1, seed);
	GradientGrid mod(detail_frequency + 1, detail_frequency + 1, ++seed);
	unsigned width = map.getWidthX();
	unsigned height = map.getWidthY();

	// Reduce the frequency of the noise a little so heightmap coordinates stay inside the grid
	float fx_base = (float)(base.width - 1) / (width + 2);
	float fy_base = (float)(base.height - 1) / (height + 2);
	float fx_mod = (float)(mod.width - 1) / (width + 2);
	float fy_mod = (float)(mod.height - 1) / (height + 2);

	// Calculate the value needed to normalize the noise
	float normalize = 0.5f / (1.0f + detail_level);

	// Apply noise
	float elevation, detail;
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			elevation = perlin(base, (x + 1) * fx_base, (y + 1) * fy_base);
			detail = perlin(mod, (x + 1) * fx_mod, (y + 1) * fy_mod);

			map.setHeight(x, y, (uint16_t)(((elevation + elevation * detail * detail_level) * normalize + 0.5f) * (max - min)) + min);
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