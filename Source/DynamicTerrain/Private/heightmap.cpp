#include "heightmap.h"

Heightmap::Heightmap()
{
	width_x = 0;
	width_y = 0;
	data = nullptr;
}

Heightmap::Heightmap(unsigned size_x, unsigned size_y)
{
	width_x = 0;
	width_y = 0;
	data = nullptr;

	resize(size_x, size_y);
}

Heightmap::~Heightmap()
{
	if (data != nullptr)
	{
		delete[] data;
	}
}

void Heightmap::resize(unsigned x, unsigned y)
{
	width_x = x;
	width_y = y;

	// Delete existing data if necessary
	if (data != nullptr)
	{
		delete[] data;
	}

	// Create the data buffer
	unsigned size = width_x * width_y;
	data = new hdata[size];
	for (unsigned i = 0; i < size; ++i)
	{
		data[i] = 0;
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