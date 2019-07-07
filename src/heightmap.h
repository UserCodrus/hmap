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

	/*
	 * Generate a heightmap using multiple layers of Perlin noise stacked on top of one another
	 *
	 * min:			The minimum elevation of the heightmap produced
	 * max:			The maximum elevation of the heightmap produced
	 * seed:		The rng seed to use for noise generation
	 * grid_size:	The size of the gradient grid used for noise generation - smaller grids mean smoother noise, while larger grids will be granier with steep elevation changes (must be > 2)
	 * octaves:		The number of layers of noise to use - lower octaves make heightmaps very rough and bumpy, while higher octaves are softer and more homogeneous (must be > 0)
	 * persistence:	The level of influence each successive octave has - higher persistence results in bumpier terrain, while lower persistence creates smooth hills (must be between 0.0 and 1.0)
	 *
	 */
	void perlinOctaves(unsigned seed, float min, float max, unsigned grid_size, unsigned octaves, float persistence);

	/*
	 * Generate a heightmap using three layers of Perlin noise
	 *
	 * min:					The minimum elevation of the heightmap produced
	 * max:					The maximum elevation of the heightmap produced
	 * seed:				The rng seed to use for noise generation
	 * base_frequency:		The frequency of the base noise - smaller frequency mean smoother terrain, while higher frequency will have steep hills (must be > 0)
	 * detail_frequency:	Affects the roughness of the terrain - low roughness produces smoother hills, and high roughness produces more bumpy, mountainous terrain (must be > 0)
	 * detail_level:		Affects the strength of the roughness - low detail means smooth, tall hills, while higher details means bumpy terrain (must be between 0.0 and 1.0)
	 *
	 */
	void perlinNotch(unsigned seed, float min, float max, unsigned base_frequency, unsigned detail_frequency, float detail_level);

	/*
	 * Generate a heightmap using three layers of Perlin noise
	 *
	 *
	 * seed:		The rng seed to use for noise generation
	 * min:			The minimum elevation of the heightmap produced
	 * max:			The maximum elevation of the heightmap produced
	 * frequency:	The frequency of the noise produced - higher frequency means granier noise
	 *
	 */
	void random(unsigned seed, float min, float max, unsigned frequency);

	void diamondSquare(unsigned seed, float min, float max, unsigned size);

	// Default heightmap generator
	void def(unsigned seed, float min, float max);

private:
	hdata* data;
	unsigned width_x;
	unsigned width_y;
};

#include "heightmap.inl"