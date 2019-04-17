#include <random>
#include <numeric>
#include <stdexcept>

#include "algorithm.h"

float lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

float grad(int hash, float x, float y)
{
	return ((hash & 1) ? x : -x) + ((hash & 2) ? y : -y);
}

void permutation(std::vector<int>& p, unsigned size, unsigned seed)
{
	// Fill the vector with sequential integers
	p.resize(size);
	std::iota(p.begin(), p.end(), 0);

	// Shuffle the permutations
	std::default_random_engine rando(seed);
	std::shuffle(p.begin(), p.end(), rando);

	// Double the vector to avoid overflow
	p.insert(p.end(), p.begin(), p.end());
}

float perlin(const std::vector<int>& p, float x, float y)
{
	// Get the size of the permutation array
	unsigned size = p.size() / 2;

	// Get the grid coordinates of the cell containing x, y
	// by flooring x and y
	int X, Y, X1, Y1;
	X = x > 0 ? x : x - 1;
	Y = y > 0 ? y : y - 1;
	X = X % size;
	Y = Y % size;
	X1 = (X + 1) % size;
	Y1 = (Y + 1) % size;
	
	// Subtract the unit coordinates of x and y to get their fractional portion
	x -= X;
	y -= Y;
	// Get the fade curves of the coordinates
	float u = fade(x);
	float v = fade(y);

	return lerp(u,
		lerp(v, grad(p[X + p[Y]], x, y),		grad(p[X + p[Y1]], x, y - 1)),
		lerp(v, grad(p[X1 + p[Y]], x - 1, y),	grad(p[X1 + p[Y1]], x - 1, y - 1))
	);
}