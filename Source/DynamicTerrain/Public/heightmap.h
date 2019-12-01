#pragma once

#include "data.h"

#include <string>

typedef float hdata;

class Vectormap
{
public:
	Vectormap();
	Vectormap(const Vectormap& _copy);
	Vectormap(unsigned x, unsigned y);
	~Vectormap();

	unsigned getWidthX() const;
	unsigned getWidthY() const;

	// Get the vector at a given location
	Vector3 getVector(unsigned x, unsigned y) const;
	// Set the vector of the heightmap at a given location
	void setVector(unsigned x, unsigned y, Vector3 value);

	// Reallocate the data array
	void resize(unsigned x, unsigned y);

private:
	Vector3* data = nullptr;
	unsigned width_x = 0;
	unsigned width_y = 0;
};

class Heightmap
{
public:
	Heightmap();
	Heightmap(const Heightmap& _copy);
	Heightmap(unsigned size_x, unsigned size_y);
	~Heightmap();

	// Reallocate the data array
	void resize(unsigned x, unsigned y);

	unsigned getWidthX() const;
	unsigned getWidthY() const;
	unsigned getSize() const;
	// Get the height at a given location
	hdata getHeight(unsigned x, unsigned y) const;
	// Set the height of the heightmap at a given location
	void setHeight(unsigned x, unsigned y, hdata value);

	// Set the heightmap to match a noise sample
	template <class T>
	void sample(T& noise, float (T::* sample)(float, float) const, float scale = 1.0f);

	// Calculate the normals and tangents for the heightmap
	void calculateNormals(Vectormap& normal, Vectormap& tangent);

	// Set the contents of the heightmap to match another heightmap
	void set(const Heightmap& in);
	// Add the height of another heightmap to this one
	void add(const Heightmap& in);
	// Subtract the height of another heightmap from this one
	void remove(const Heightmap& in);
	// Multiply the height of each point by the height of another heightmap
	void multiply(const Heightmap& in);

	// Set the contents of the heightmap to a constant value
	void set(const float c);
	// Add a constant value to the height of the heightmap
	void add(const float c);
	// Subtract a constant value to the height of the heightmap
	void remove(const float c);
	// Multiply each height value by a constant
	void multiply(const float c);
	// Divide each height value by a constant
	void divide(const float c);

private:
	hdata* data = nullptr;
	unsigned width_x = 0;
	unsigned width_y = 0;
};

#include "heightmap.inl"