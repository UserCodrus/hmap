#include "algorithm.h"

#include <random>
#include <stdexcept>

constexpr float pi = 3.141592f;

///
/// Utility functions
///

// Linear interpolation
inline float lerp(float t, float a, float b)
{
	return a + t * (b - a);
}

// Cosine interpolation
inline float corp(float t, float a, float b)
{
	float u = (1 - std::cos(t * pi)) / 2;
	return a * (1 - u) + b * u;
}

// Cubic interpolation
inline float curp(float t, float a[4])
{
	return a[1] + 0.5f * t * (a[2] - a[0] + t * (2.0f * a[0] - 5.0f * a[1] + 4.0f * a[2] - a[3] + t * (3.0f * (a[1] - a[2]) + a[3] - a[0])));
}

// Perlin smoothstep
inline float fade(float t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

///
/// Base noise
///

unsigned Noise::getWidth()
{
	return width;
}

unsigned Noise::getHeight()
{
	return height;
}

///
/// Perlin and simplex noise
///

GradientNoise::GradientNoise(unsigned _width, unsigned _height, unsigned seed)
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

GradientNoise::~GradientNoise()
{
	if (gradient != nullptr)
	{
		delete[] gradient;
	}
}

Vector2 GradientNoise::getGradient(unsigned x, unsigned y) const
{
	return gradient[y * width + x];
}

float GradientNoise::perlin(float x, float y) const
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

ValueNoise::ValueNoise()
{
	// Currently unused
	width = 0;
	height = 0;
}

ValueNoise::ValueNoise(unsigned _width, unsigned _height, unsigned seed)
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

ValueNoise::~ValueNoise()
{
	if (value != nullptr)
	{
		delete[] value;
	}
}

float ValueNoise::getValue(unsigned x, unsigned y) const
{
	return value[y * width + x];
}

float ValueNoise::linear(float x, float y) const
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

float ValueNoise::cosine(float x, float y) const
{
	// Get the coordinates of the grid cell containing x, y
	int X = (int)x;
	int Y = (int)y;

	// Subtract the cell coordinates from x and y to get their fractional portion
	x -= X;
	y -= Y;

	// Interpolate the noise
	return corp(x,
		corp(y, value[Y * width + X], value[(Y + 1) * width + X]),
		corp(y, value[Y * width + (X + 1)], value[(Y + 1) * width + (X + 1)])
	);
}

float ValueNoise::cubic(float x, float y) const
{
	// Get the coordinates of the grid cell containing x, y
	int X = (int)x;
	int Y = (int)y;

	// Subtract the cell coordinates from x and y to get their fractional portion
	x -= X;
	y -= Y;

	float a[4], p[4];

	// Second point
	p[1] = value[Y * width + X];
	p[2] = value[Y * width + (X + 1)];
	if (X > 0)
	{
		p[0] = value[Y * width + (X - 1)];
	}
	else
	{
		// Extrapolate the middle points if we are near an edge
		p[0] = p[1] - (p[2] - p[1]);
	}
	if (X < (int)(width - 2))
	{
		p[3] = value[Y * width + (X + 2)];
	}
	else
	{
		// Extrapolate the middle points if we are near an edge
		p[3] = p[2] + (p[2] - p[1]);
	}
	a[1] = curp(x, p);

	// Third point
	p[1] = value[(Y + 1) * width + X];
	p[2] = value[(Y + 1) * width + (X + 1)];
	if (X > 0)
	{
		p[0] = value[(Y + 1) * width + (X - 1)];
	}
	else
	{
		// Extrapolate the middle points if we are near an edge
		p[0] = p[1] - (p[2] - p[1]);
	}
	if (X < (int)(width - 2))
	{
		p[3] = value[(Y + 1) * width + (X + 2)];
	}
	else
	{
		// Extrapolate the middle points if we are near an edge
		p[3] = p[2] + (p[2] - p[1]);
	}
	a[2] = curp(x, p);

	// First point
	if (Y > 0)
	{
		p[1] = value[(Y - 1) * width + X];
		p[2] = value[(Y - 1) * width + (X + 1)];
		if (X > 0)
		{
			p[0] = value[(Y - 1) * width + (X - 1)];
		}
		else
		{
			// Extrapolate the middle points if we are near an edge
			p[0] = p[1] - (p[2] - p[1]);
		}
		if (X < (int)(width - 2))
		{
			p[3] = value[(Y - 1) * width + (X + 2)];
		}
		else
		{
			// Extrapolate the middle points if we are near an edge
			p[3] = p[2] + (p[2] - p[1]);
		}
		a[0] = curp(x, p);
	}
	else
	{
		// Extrapolate the middle points if we are near an edge
		a[0] = a[1] - (a[2] - a[1]);
	}

	// Fourth point
	if (Y < (int)(height - 2))
	{
		p[1] = value[(Y + 2) * width + X];
		p[2] = value[(Y + 2) * width + (X + 1)];
		if (X > 0)
		{
			p[0] = value[(Y + 2) * width + (X - 1)];
		}
		else
		{
			// Extrapolate the middle points if we are near an edge
			p[0] = p[1] - (p[2] - p[1]);
		}
		if (X < (int)(width - 2))
		{
			p[3] = value[(Y + 2) * width + (X + 2)];
		}
		else
		{
			// Extrapolate the middle points if we are near an edge
			p[3] = p[2] + (p[2] - p[1]);
		}
		a[3] = curp(x, p);
	}
	else
	{
		// Extrapolate the middle points if we are near an edge
		a[3] = a[2] + (a[2] - a[1]);
	}

	return std::min(1.0f, std::max(curp(y, a), -1.0f));
}

PlasmaNoise::PlasmaNoise(unsigned size, unsigned seed)
{
	width = (unsigned)pow(2, size) + 1;
	height = width;
	value = new float[width * height];

	std::default_random_engine rando(seed);
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	// The range of the random values generated
	float range = 0.5;

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
		range *= 0.5;
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