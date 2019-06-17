#include "heightmap.h"
#include "algorithm.h"

#include <limits>

void Heightmap::perlinOctaves(unsigned seed, float min, float max, unsigned grid_size, unsigned octaves, float persistence)
{
	// Create the gradient grid
	GradientGrid grad(grid_size, grid_size, seed);
	//unsigned width = map.getWidthX();
	//unsigned height = map.getWidthY();

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

	// Get the limits of the heightmap
	float bottom = min * std::numeric_limits<hdata>::max();
	float delta = (max - min) * std::numeric_limits<hdata>::max();

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
			float fx = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / (width_x + 2));
			float fy = (1.0f / (float)pow(2, octaves - 1)) * ((float)(grid_size - 1) / (width_y + 2));
			
			for (unsigned o = 0; o < octaves; ++o)
			{
				noise += grad.perlin((x + 1) * fx, (y + 1) * fy) * amplitude;
				total_amplitude += amplitude;
				amplitude *= persistence;
				fx *= 2;
				fy *= 2;
			}

			noise = (noise / total_amplitude + 1.0f) * 0.5f;
			setHeight(x, y, (hdata)(noise * delta + bottom));
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
	GradientGrid base(base_frequency + 1, base_frequency + 1, seed);
	GradientGrid mod(detail_frequency + 1, detail_frequency + 1, ++seed);
	//unsigned width = map.getWidthX();
	//unsigned height = map.getWidthY();

	// Reduce the frequency of the noise a little so heightmap coordinates stay inside the grid
	float fx_base = (float)(base.getWidth() - 1) / (width_x + 2);
	float fy_base = (float)(base.getHeight() - 1) / (width_y + 2);
	float fx_mod = (float)(mod.getWidth() - 1) / (width_x + 2);
	float fy_mod = (float)(mod.getHeight() - 1) / (width_y + 2);

	// Calculate the value needed to normalize the noise
	float normalize = 0.5f / (1.0f + detail_level);

	// Get the limits of the heightmap
	float bottom = min * std::numeric_limits<hdata>::max();
	float delta = (max - min) * std::numeric_limits<hdata>::max();

	// Apply noise
	float elevation, detail;
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			elevation = base.perlin((x + 1) * fx_base, (y + 1) * fy_base);
			detail = mod.perlin((x + 1) * fx_mod, (y + 1) * fy_mod);

			setHeight(x, y, (hdata)(((elevation + elevation * detail * detail_level) * normalize + 0.5f) * delta + bottom));
		}
	}
}

void Heightmap::random(unsigned seed, float min, float max, unsigned frequency)
{
	// Generate noise
	ValueGrid grid(frequency, frequency, seed);

	// Add frequency constants so the heightmap coordinates stay in the grid
	float fx = (float)(grid.getWidth() - 1) / width_x;
	float fy = (float)(grid.getHeight() - 1) / width_y;

	// Get the limits of the heightmap
	float bottom = min * std::numeric_limits<hdata>::max();
	float delta = (max - min) * std::numeric_limits<hdata>::max();

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			setHeight(x, y, (hdata)((grid.noise(x * fx, y * fy) * 0.5f + 0.5f) * delta + bottom));
		}
	}
}

void Heightmap::diamondSquare(unsigned seed, float min, float max, unsigned size)
{
	// Generate noise
	ValueGrid grid(size, seed);

	// Add frequency constants so the heightmap coordinates stay in the grid
	float fx = (float)(grid.getWidth() - 1) / width_x;
	float fy = (float)(grid.getHeight() - 1) / width_y;

	// Get the limits of the heightmap
	float bottom = min * std::numeric_limits<hdata>::max();
	float delta = (max - min) * std::numeric_limits<hdata>::max();

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			setHeight(x, y, (hdata)((grid.noise(x * fx, y * fy) * 0.5f + 0.5f) * delta + bottom));
		}
	}
}