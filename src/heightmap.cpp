#include "heightmap.h"

Heightmap::Heightmap()
{
	width_x = 0;
	width_y = 0;
	data = nullptr;
}

Heightmap::Heightmap(const Heightmap& _copy)
{
	width_x = 0;
	width_y = 0;
	data = nullptr;

	resize(_copy.width_x , _copy.width_y);
	memcpy(data, _copy.data, width_x * width_y);
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

///
/// Heightmap arithmatic
///

void Heightmap::set(const Heightmap& in)
{
	unsigned size_x = 0;
	unsigned size_y = 0;

	// Don't go outside of either heightmap
	if (in.width_x < width_x)
	{
		size_x = in.width_x;
	}
	else
	{
		size_x = width_x;
	}
	if (in.width_y < width_y)
	{
		size_y = in.width_y;
	}
	else
	{
		size_y = width_y;
	}

	// Copy height data from the other map
	for (unsigned y = 0; y < size_y; ++y)
	{
		for (unsigned x = 0; x < size_x; ++x)
		{
			data[y * width_x + x] = in.data[y * in.width_x + x];
		}
	}
}

void Heightmap::add(const Heightmap& in)
{
	unsigned size_x = 0;
	unsigned size_y = 0;

	// Don't go outside of either heightmap
	if (in.width_x < width_x)
	{
		size_x = in.width_x;
	}
	else
	{
		size_x = width_x;
	}
	if (in.width_y < width_y)
	{
		size_y = in.width_y;
	}
	else
	{
		size_y = width_y;
	}

	// Add the height values from the other heightmap
	for (unsigned y = 0; y < size_y; ++y)
	{
		for (unsigned x = 0; x < size_x; ++x)
		{
			data[y * width_x + x] += in.data[y * in.width_x + x];
		}
	}
}

void Heightmap::remove(const Heightmap& in)
{
	unsigned size_x = 0;
	unsigned size_y = 0;

	// Don't go outside of either heightmap
	if (in.width_x < width_x)
	{
		size_x = in.width_x;
	}
	else
	{
		size_x = width_x;
	}
	if (in.width_y < width_y)
	{
		size_y = in.width_y;
	}
	else
	{
		size_y = width_y;
	}

	// Subtract the height values from the other heightmap
	for (unsigned y = 0; y < size_y; ++y)
	{
		for (unsigned x = 0; x < size_x; ++x)
		{
			data[y * width_x + x] -= in.data[y * in.width_x + x];
		}
	}
}

void Heightmap::multiply(const Heightmap& in)
{
	unsigned size_x = 0;
	unsigned size_y = 0;

	// Don't go outside of either heightmap
	if (in.width_x < width_x)
	{
		size_x = in.width_x;
	}
	else
	{
		size_x = width_x;
	}
	if (in.width_y < width_y)
	{
		size_y = in.width_y;
	}
	else
	{
		size_y = width_y;
	}

	// Multiply the height values of each heightmap
	for (unsigned y = 0; y < size_y; ++y)
	{
		for (unsigned x = 0; x < size_x; ++x)
		{
			data[y * width_x + x] *= in.data[y * in.width_x + x];
		}
	}
}

///
/// Constant arithmatic
///

void Heightmap::set(const float c)
{
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] = c;
		}
	}
}

void Heightmap::add(const float c)
{
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] += c;
		}
	}
}

void Heightmap::remove(const float c)
{
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] -= c;
		}
	}
}

void Heightmap::multiply(const float c)
{
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] *= c;
		}
	}
}

void Heightmap::divide(const float c)
{
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] /= c;
		}
	}
}