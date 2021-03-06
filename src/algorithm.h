#pragma once

#include "data.h"

#include <vector>
#include <stack>
#include <algorithm>

// Linear interpolation
inline float lerp(float t, float a, float b);
// Cosine interpolation
inline float corp(float t, float a, float b);
// Cubic interpolation
inline float curp(float t, float a[4]);
// Perlin smoothstep function
inline float fade(float t);

// The square of the distance between two vectors in the x and y dimensions
template <class T>
float distance2D(const T& a, const T& b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;

	return dx * dx + dy * dy;
}

class Noise
{
public:
	unsigned getWidth() const;
	unsigned getHeight() const;

	virtual void scale(unsigned sample_width, unsigned sample_height) = 0;

protected:
	unsigned width = 0;
	unsigned height = 0;

	float scale_x = 0.0f;
	float scale_y = 0.0f;
};

// Noise generated by creating a grid of gradient vectors
class GradientNoise : public Noise
{
public:
	GradientNoise(unsigned _width, unsigned _height, unsigned seed);
	GradientNoise(const GradientNoise& copy);
	virtual ~GradientNoise();

	virtual void scale(unsigned sample_width, unsigned sample_height) override;

	// Get the gradient at a given grid point
	Vector2 getGradient(unsigned x, unsigned y) const;

	// Get Perlin noise at the specified coordinate
	float perlin(float x, float y) const;

protected:
	Vector2* gradient = nullptr;
};

// Noise generated by creating a grid of random values
class ValueNoise : public Noise
{
public:
	ValueNoise() {};
	ValueNoise(unsigned _width, unsigned _height, unsigned seed);
	ValueNoise(const ValueNoise& copy);
	virtual ~ValueNoise();

	virtual void scale(unsigned sample_width, unsigned sample_height) override;

	// Get the value at a given grid point
	float getValue(unsigned x, unsigned y) const;

	// Bilinear interpolated noise
	virtual float linear(float x, float y) const;
	// Cosine interpolated noise
	virtual float cosine(float x, float y) const;
	// Cubic interpolated noise
	virtual float cubic(float x, float y) const;

protected:
	float* value = nullptr;
};

// A value noise grid created using the diamond square algorithm
class PlasmaNoise : public ValueNoise
{
public:
	PlasmaNoise(unsigned size, unsigned seed);
	PlasmaNoise(const PlasmaNoise& copy);
};

// Noise generated by choosing random points in a given area
class PointNoise : public Noise
{
public:
	PointNoise() {};
	PointNoise(unsigned x_bias, unsigned y_bias, unsigned points, unsigned seed);
	PointNoise(const PointNoise& copy);
	virtual ~PointNoise();

	virtual void scale(unsigned sample_width, unsigned sample_height) override;

	// Get the nearest point to a given location
	virtual inline Vector2 getNearest(Vector2 location) const;

	// Sample point noise at the given coordinates
	virtual float dot(float x, float y) const;
	// Sample raw Worley noise at the given coordinates
	virtual float worley(float x, float y) const;

private:
	// Get the closest point to the provided point in the given cell
	//inline void getNearestPoint(Vector2 location, Vector2& nearest, float& distance);

	std::vector<Vector2>* points = nullptr;
	unsigned array_size = 0;
};

// Noise generated by plotting random points within each cell of a unit grid
class GridNoise : public PointNoise
{
public:
	GridNoise(unsigned _width, unsigned _height, unsigned seed);
	GridNoise(const GridNoise& copy);
	virtual ~GridNoise();

	// Get the point in the provided grid cell
	inline Vector2 getPoint(unsigned x, unsigned y) const;
	// Get the nearest point to the provided coordinates
	virtual inline Vector2 getNearest(Vector2 location) const override;

	// Sample point noise at the given coordinates
	virtual float dot(float x, float y) const override;
	// Sample raw Worley noise at the given coordinates
	virtual float worley(float x, float y) const override;

private:
	// Get the closest point to the provided point in the given cell
	//inline void getNearestPoint(Vector2 location, Vector2& nearest, float& distance);

	Vector2* points = nullptr;
	unsigned array_size = 0;
};