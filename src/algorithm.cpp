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
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
	for (unsigned y = 0; y < height; ++y)
	{
		for (unsigned x = 0; x < width; ++x)
		{
			value[y * width + x] = dist(rando);
		}
	}
}

ValueGrid::ValueGrid(unsigned size, float decay, unsigned seed)
{
	width = (unsigned)pow(2, size) + 1;
	height = width;
	value = new float[width * height];

	std::default_random_engine rando(seed);
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	if (decay > 1.0f)
	{
		decay = 1.0f;
	}
	else if (decay < 0.0f)
	{
		decay = 0.0f;
	}

	// The range of the random values generated
	// Equal to decay ^ x where x is the current permutation
	float range = 0.5f;

	// Generate corner values
	value[0] = dist(rando) * range;
	value[width - 1] = dist(rando) * range;
	value[(height - 1) * width] = dist(rando) * range;
	value[height * width - 1] = dist(rando) * range;

	// The size of the current fractal
	unsigned stride = width - 1;
	
	unsigned limit_x = width - 1;
	unsigned limit_y = height - 1;

	// Diamond-square algorithm
	while (stride > 1)
	{
		range *= 0.5f;
		unsigned half = stride / 2;

		// Diamond step
		for (unsigned y = 0; y < limit_y; y += stride)
		{
			for (unsigned x = 0; x < limit_x; x += stride)
			{
				// Get the values of the four grid points at the corner of the current grid point
				float v00 = value[y * width + x];
				float v10 = value[y * width + (x + stride)];
				float v01 = value[(y + stride) * width + x];
				float v11 = value[(y + stride) * width + (x + stride)];

				// Set the value of the current point to the average of the corners + a random value
				value[(y + half) * width + (x + half)] = (v00 + v10 + v01 + v11) / 4.0f + dist(rando) * range;
			}
		}

		

		// Square step - top / bottom edges
		for (unsigned x = half; x < limit_x; x += stride)
		{
			// Get the values for the points adjacent to the top edge
			float v_mid = value[half * width + x];
			float v_left = value[x - half];
			float v_right = value[x + half];

			// Set the value for the top edge at the current x coordinate to the average of the adjacent points + a random value
			value[x] = (v_mid + v_left + v_right) / 3.0f + dist(rando) * range;

			// Get the values for the points adjacent to the bottom edge
			v_mid = value[(limit_y - half) * width + x];
			v_left = value[limit_y * width + (x - half)];
			v_right = value[limit_y * width + (x + half)];

			// Set the value for the bottom edge at the current x coordinate to the average of the adjacent points + a random value
			value[limit_y * width + x] = (v_mid + v_left + v_right) / 3.0f + dist(rando) * range;
		}

		// Square step - left / right edges
		for (unsigned y = half; y < limit_y; y += stride)
		{
			// Get the values for the points adjacent to the left edge
			float v_mid = value[y * width + half];
			float v_top = value[(y - half) * width];
			float v_bottom = value[(y + half) * width];

			// Set the value for the left edge at the current y coordinate to the average of the adjacent points + a random value
			value[y * width] = (v_mid + v_top + v_bottom) / 3.0f + dist(rando) * range;

			// Get the values for the points adjacent to the right edge
			v_mid = value[y * width + (limit_x - half)];
			v_top = value[(y - half) * width + limit_x];
			v_bottom = value[(y + half) * width + limit_x];

			// Set the value for the right edge at the current y coordinate to the average of the adjacent points + a random value
			value[y * width + limit_x] = (v_mid + v_top + v_bottom) / 3.0f + dist(rando) * range;
		}

		// Square step - center points
		bool offset = true;
		for (unsigned y = half; y < limit_y; y += half)
		{
			for (unsigned x = offset ? stride : half; x < limit_x; x += stride)
			{
				// Get the values of the four grid points adjacent the current grid point
				float v_top = value[(y - half) * width + x];
				float v_bottom = value[(y + half) * width + x];
				float v_left = value[y * width + (x - half)];
				float v_right = value[y * width + (x + half)];

				// Set the value of the current point to the average of the adjacent points + a random value
				value[y * width + x] = (v_top + v_bottom + v_left + v_right) / 4.0f + dist(rando) * range;
			}

			offset = !offset;
		}

		stride /= 2;
	}
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