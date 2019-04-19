#include <random>
#include <numeric>
#include <stdexcept>
#include <math.h>

#include "algorithm.h"

constexpr float pi = 3.141592f;

/// Math functions

float lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

/// Data structures

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

unsigned GradientGrid::getWidth() const
{
	return width;
}

unsigned GradientGrid::getHeight() const
{
	return height;
}

Vector2 GradientGrid::getGradient(unsigned x, unsigned y) const
{
	return gradient[y * width + x];
}

/// Noise algorithms

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

// A hashed gradient function for perlin noise
float grad(int hash, float x, float y)
{
	// Produce a gradient vector based on the last four bits of the hash
	switch (hash & 0x7)
	{
	case 0x0:
		return x + y;
	case 0x1:
		return -x + y;
	case 0x2:
		return x - y;
	case 0x3:
		return -x - y;
	case 0x4:
		return x;
	case 0x5:
		return -x;
	case 0x6:
		return y;
	case 0x7:
		return -y;
	}

	return 0;
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

float perlin(const GradientGrid& g, float x, float y)
{
	// Get to coordinates of the grid cell containing x, y
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

	// Get the gradients at the corner of the unit cell
	Vector2 g00 = g.getGradient(X, Y);
	Vector2 g01 = g.getGradient(X, Y1);
	Vector2 g10 = g.getGradient(X1, Y);
	Vector2 g11 = g.getGradient(X1, Y1);

	return lerp(u,
		lerp(v, g00.x * x + g00.y * y, g01.x * x + g01.y * (y - 1)),
		lerp(v, g10.x * (x - 1) + g10.y * y, g11.x * (x - 1) + g11.y * (y - 1))
	);
}

float perlinT(const GradientGrid& g, float x, float y)
{
	int X, Y, X1, Y1;

	// Floor the coordinates to get the grid cell containing x and y
	X = (int)x;
	Y = (int)y;

	// Subtract the unit coordinates of x and y to get their fractional portion
	x -= X;
	y -= Y;

	// Wrap the grid coordinates and get the coordinates for the opposite corner
	X = X % g.getWidth();
	Y = Y % g.getHeight();
	X1 = (X + 1) % g.getWidth();
	Y1 = (Y + 1) % g.getHeight();

	// Get the fade curves of the coordinates
	float u = fade(x);
	float v = fade(y);

	// Interpolate the dot products of the gradients
	return lerp(u,
		lerp(v, g.getGradient(X, Y).x * x + g.getGradient(X, Y).y * y, g.getGradient(X, Y1).x * x + g.getGradient(X, Y1).y * (y - 1)),
		lerp(v, g.getGradient(X1, Y).x * (x - 1) + g.getGradient(X1, Y).y * y, g.getGradient(X1, Y1).x * (x - 1) + g.getGradient(X1, Y1).y * (y - 1))
	);
}