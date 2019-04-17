#include "generate.h"
#include "algorithm.h"

void layeredPerlin(Heightmap<uint16_t>& map, unsigned seed, float frequency, unsigned octaves, uint16_t minimum, uint16_t maximum)
{
	const unsigned perms = 256;
	uint16_t delta = maximum - minimum;

	// Generate the permutations
	std::vector<int> p;
	permutation(p, perms, seed);

	// Determine the frequency of the heightmap
	if (frequency < 1.0f)
	{
		frequency = 1.0f;
	}
	float fx = (map.getWidthX() / perms + 2) * frequency;
	float fy = (map.getWidthY() / perms + 2) * frequency;

	for (unsigned y = 0; y < map.getWidthY(); ++y)
	{
		for (unsigned x = 0; x < map.getWidthX(); ++x)
		{
			float noise = 1.0f;
			for (unsigned o = 1; o <= octaves; ++o)
			{
				noise *= (perlin(p, x / (fx * o) + 1, y / (fy * o) + 1) + 1.0f) * 0.5f;
			}
			map.setData(x, y, (uint16_t)(noise * delta) + maximum);
		}
	}
}