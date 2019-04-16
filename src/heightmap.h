#pragma once

#include <string>

enum class U16 : uint16_t
{
	MINIMUM = 0x0000,	// The lowest possible value
	QUARTER = 0x3FFF,	// 1/4 of the maximum value
	THIRD = 0x5555,		// 1/3 of the maximum value
	HALF = 0x7FFF,		// 1/2 of the maximum value
	MAXIMUM = 0xFFFF	// The maximum value
};

template <typename hdata>
class Heightmap
{
public:
	Heightmap(unsigned size_x, unsigned size_y);
	~Heightmap();

	unsigned getWidthX() const;
	unsigned getWidthY() const;
	unsigned getSize() const;
	hdata getData(unsigned x, unsigned y) const;

	void setData(unsigned x, unsigned y, hdata value);

private:
	hdata* data = nullptr;
	unsigned width_x;
	unsigned width_y;
};

#include "heightmap.inl"