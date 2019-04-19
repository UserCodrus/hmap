#pragma once

#include <vector>

/// Math functions

// Linear interpolation
inline float lerp(float t, float a, float b);
// Perlin fade
inline float fade(float t);

/// Data structures

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;
};

// A gradient grid filled with random unit vectors
class GradientGrid
{
public:
	GradientGrid(unsigned _width, unsigned _height, unsigned seed);
	~GradientGrid();

	unsigned getWidth() const;
	unsigned getHeight() const;
	Vector2 getGradient(unsigned x, unsigned y) const;

private:
	unsigned width;
	unsigned height;
	Vector2* gradient = nullptr;
};

/// Noise algorithms

// Generate a set of permutations for hashed perlin noise gradients
void permutation(std::vector<int>& p, unsigned size, unsigned seed);
// Perlin noise function with hashed gradients
// A permutation array must be provided, and the coordinates must be greater than 0
float perlin(const int* p, float x, float y);
// Perlin noise function with a gradient grid
// A permutation array must be provided, and the coordinates must be greater than 0
float perlin(const GradientGrid& g, float x, float y);