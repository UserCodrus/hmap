#pragma once

#include <vector>

// Linear interpolation
inline float lerp(float t, float a, float b);
// Perlin smoothstep function
inline float fade(float t);

struct Vector2
{
	float x = 0.0f;
	float y = 0.0f;
};

// A gradient grid filled with random unit vectors
// Used to generate Perlin noise and simplex noise
struct GradientGrid
{
public:
	GradientGrid(unsigned _width, unsigned _height, unsigned seed);
	~GradientGrid();

	Vector2 getGradient(unsigned x, unsigned y) const;
	unsigned getWidth();
	unsigned getHeight();

	// Get Perlin noise at the specified coordinate
	float perlin(float x, float y) const;

private:
	unsigned width;
	unsigned height;
	Vector2* gradient = nullptr;
};

// Generate a set of permutations for hashed perlin noise gradients
void permutation(std::vector<int>& p, unsigned size, unsigned seed);

// Classic Perlin noise function with hashed gradients
float perlin(const int* p, float x, float y);