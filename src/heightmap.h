#pragma once

#include <string>

typedef uint16_t hdata;

class Heightmap
{
public:
	Heightmap(unsigned size_x, unsigned size_y);
	~Heightmap();

	unsigned getWidthX() const;
	unsigned getWidthY() const;
	unsigned getSize() const;
	hdata getHeight(unsigned x, unsigned y) const;

	void setHeight(unsigned x, unsigned y, hdata value);

private:
	hdata* data = nullptr;
	unsigned width_x;
	unsigned width_y;
};