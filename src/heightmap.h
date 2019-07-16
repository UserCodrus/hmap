#pragma once

#include <string>

typedef float hdata;

class Heightmap
{
public:
	Heightmap();
	Heightmap(const Heightmap& _copy);
	Heightmap(unsigned size_x, unsigned size_y);
	~Heightmap();

	// Reallocate the internal array
	void resize(unsigned x, unsigned y);

	unsigned getWidthX() const;
	unsigned getWidthY() const;
	unsigned getSize() const;
	hdata getHeight(unsigned x, unsigned y) const;
	void setHeight(unsigned x, unsigned y, hdata value);

	// Set the heightmap to match a noise sample
	template <class T>
	void set(const T& noise, float (T::* sample)(float, float) const, float scale = 1.0f);
	// Add a noise sample to the height of the heightmap
	template <class T>
	void add(const T& noise, float (T::*sample)(float, float) const, float scale = 1.0f);
	// Multiply the heightmap by a noise sample
	template <class T>
	void multiply(const T& noise, float (T::* sample)(float, float) const, float scale = 1.0f);

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
	hdata* data;
	unsigned width_x;
	unsigned width_y;
};

#include "heightmap.inl"