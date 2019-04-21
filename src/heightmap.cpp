#include "heightmap.h"

Heightmap::Heightmap(unsigned size_x, unsigned size_y)
{
	width_x = size_x;
	width_y = size_y;

	// Create the data buffer
	unsigned size = width_x * width_y;
	data = new hdata[size];
	for (unsigned i = 0; i < size; ++i)
	{
		data[i] = 0;
	}
}

Heightmap::~Heightmap()
{
	if (data != nullptr)
	{
		delete[] data;
	}
}

unsigned Heightmap::getWidthX() const
{
	return width_x;
}

unsigned Heightmap::getWidthY() const
{
	return width_y;
}

unsigned Heightmap::getSize() const
{
	return sizeof(hdata);
}

hdata Heightmap::getHeight(unsigned x, unsigned y) const
{
	if (x >= width_x || y >= width_y)
	{
		throw new std::exception("Array out of bounds in heightmap");
	}

	return data[y * width_x + x];
}

void Heightmap::setHeight(unsigned x, unsigned y, hdata value)
{
	if (x >= width_x || y >= width_y)
	{
		throw new std::exception("Array out of bounds in heightmap");
	}

	data[y * width_x + x] = value;
}