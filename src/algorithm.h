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
class GradientNoise
{
public:
	GradientNoise(unsigned _width, unsigned _height, unsigned seed);
	~GradientNoise();

	Vector2 getGradient(unsigned x, unsigned y) const;
	unsigned getWidth();
	unsigned getHeight();

	// Get Perlin noise at the specified coordinate
	float perlin(float x, float y) const;

protected:
	unsigned width;
	unsigned height;
	Vector2* gradient = nullptr;
};

// A value noise grid with random values at each point
class ValueNoise
{
public:
	ValueNoise();
	ValueNoise(unsigned _width, unsigned _height, unsigned seed);
	~ValueNoise();

	float getValue(unsigned x, unsigned y) const;
	unsigned getWidth();
	unsigned getHeight();

	// Bilinear interpolated noise
	float linear(float x, float y) const;
	// Cosine interpolated noise
	float cosine(float x, float y) const;
	// Cubic interpolated noise
	float cubic(float x, float y) const;

protected:
	unsigned width;
	unsigned height;
	float* value = nullptr;
};

// A value noise grid created using the diamond square algorithm
class PlasmaNoise : public ValueNoise
{
public:
	PlasmaNoise(unsigned size, unsigned seed);
};

// Generate a set of permutations for hashed perlin noise gradients
void permutation(std::vector<int>& p, unsigned size, unsigned seed);

// Classic Perlin noise function with hashed gradients
float perlin(const int* p, float x, float y);