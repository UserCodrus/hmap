#include "algorithm.h"

#include <random>
#include <numeric>
#include <stdexcept>
#include <cmath>
#include <iostream>

constexpr float pi = 3.141592f;

///
/// Utility functions
///

// Linear interpolation
float lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

// Perlin smoothstep
float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

// Hashed perlin gradient
float grad(int hash, float x, float y)
{
	// Produce a gradient vector based on the last two bits of the hash
	switch (hash & 0x3)
	{
	case 0x0:
		return x + y;
	case 0x1:
		return -x + y;
	case 0x2:
		return x - y;
	case 0x3:
		return -x - y;
	}

	return 0;
}

///
/// Perlin and simplex noise
///

GradientGrid::GradientGrid(unsigned _width, unsigned _height, unsigned seed)
{
	width = _width;
	height = _height;
	gradient = new Vector2[width * height];

	// Generate gradient vectors
	std::default_random_engine rando(seed);
	std::uniform_real_distribution<float> dist(-pi, pi);
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			// Create a unit vector from a random angle
			float angle = dist(rando);
			gradient[y * width + x].x = cos(angle);
			gradient[y * width + x].y = sin(angle);
		}
	}
}

GradientGrid::~GradientGrid()
{
	if (gradient != nullptr)
	{
		delete[] gradient;
	}
}

Vector2 GradientGrid::getGradient(unsigned x, unsigned y) const
{
	return gradient[y * width + x];
}

unsigned GradientGrid::getWidth()
{
	return width;
}

unsigned GradientGrid::getHeight()
{
	return height;
}

float GradientGrid::perlin(float x, float y) const
{
	// Get the coordinates of the grid cell containing x, y
	int X = (int)x;
	int Y = (int)y;

	// Subtract the cell coordinates from x and y to get their fractional portion
	x -= X;
	y -= Y;

	// Get the fade curves of the coordinates
	float u = fade(x);
	float v = fade(y);

	// Get the gradients at the corner of the unit cell
	Vector2 g00 = gradient[Y * width + X];
	Vector2 g01 = gradient[(Y + 1) * width + X];
	Vector2 g10 = gradient[Y * width + X + 1];
	Vector2 g11 = gradient[(Y + 1) * width + X + 1];

	// Interpolate the dot products of each gradient and the cell coordinates
	return lerp(u,
		lerp(v, g00.x * x + g00.y * y,			g01.x * x + g01.y * (y - 1)),
		lerp(v, g10.x * (x - 1) + g10.y * y,	g11.x * (x - 1) + g11.y * (y - 1))
	);
}

///
/// Value & diamond square noise
///

ValueGrid::ValueGrid(unsigned _width, unsigned _height, unsigned seed)
{
	width = _width;
	height = _height;
	value = new float[width * height];

	// Generate random values at each grid point
	std::default_random_engine rando(seed);
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			value[y * width + x] = dist(rando);
		}
	}
}

ValueGrid::ValueGrid(unsigned size, unsigned seed)
{
	width = (unsigned)pow(2, size) + 1;
	height = width;
	value = new float[width * height];

	// Generate diamond square values
}

ValueGrid::~ValueGrid()
{
	if (value != nullptr)
	{
		delete[] value;
	}
}

float ValueGrid::getValue(unsigned x, unsigned y) const
{
	return value[y * width + x];
}

unsigned ValueGrid::getWidth()
{
	return width;
}

unsigned ValueGrid::getHeight()
{
	return height;
}

float ValueGrid::noise(float x, float y) const
{
	// Get the coordinates of the grid cell containing x, y
	int X = (int)x;
	int Y = (int)y;

	// Subtract the cell coordinates from x and y to get their fractional portion
	x -= X;
	y -= Y;

	// Interpolate the noise
	return lerp(x,
		lerp(y, value[Y * width + X],			value[(Y + 1) * width + X]),
		lerp(y, value[Y * width + (X + 1)],		value[(Y + 1) * width + (X + 1)])
	);
}

///
/// Classic perlin noise
///

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

float perlin(const int* p, float x, float y)
{
	// Get the grid coordinates of the cell containing x, y
	int X, Y, X1, Y1;
	X = (int)x;
	Y = (int)y;
	X1 = X + 1;
	Y1 = Y + 1;
	
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