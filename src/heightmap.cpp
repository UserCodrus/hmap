#include "heightmap.h"

Vectormap::Vectormap()
{

}

Vectormap::Vectormap(const Vectormap& _copy)
{
	resize(_copy.width_x, _copy.width_y);
	memcpy(data, _copy.data, width_x * width_y * sizeof(hdata));
}

Vectormap::Vectormap(unsigned x, unsigned y)
{
	resize(x, y);
}

Vectormap::~Vectormap()
{
	if (data != nullptr)
	{
		delete[] data;
	}
}

unsigned Vectormap::getWidthX() const
{
	return width_x;
}

unsigned Vectormap::getWidthY() const
{
	return width_y;
}

Vector3 Vectormap::getVector(unsigned x, unsigned y) const
{
	return data[y * width_x + x];
}

void Vectormap::setVector(unsigned x, unsigned y, Vector3 value)
{
	data[y * width_x + x] = value;
}

void Vectormap::resize(unsigned x, unsigned y)
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
	data = new Vector3[size];
}

Heightmap::Heightmap()
{

}

Heightmap::Heightmap(const Heightmap& _copy)
{
	resize(_copy.width_x , _copy.width_y);
	memcpy(data, _copy.data, width_x * width_y * sizeof(hdata));
}

Heightmap::Heightmap(unsigned size_x, unsigned size_y)
{
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
	return data[y * width_x + x];
}

void Heightmap::setHeight(unsigned x, unsigned y, hdata value)
{
	data[y * width_x + x] = value;
}

void Heightmap::calculateNormals(Vectormap& normal, Vectormap& tangent, float scale)
{
	// Make sure the normal and tangent vector maps are the same size as the heightmap
	if (normal.getWidthX() != width_x || normal.getWidthY() != width_y)
	{
		normal.resize(width_x, width_y);
	}
	if (tangent.getWidthX() != width_x || tangent.getWidthY() != width_y)
	{
		tangent.resize(width_x, width_y);
	}

	// Change the scaling factor
	if (scale <= 0.0f)
	{
		if (width_x > width_y)
		{
			scale = width_y;
		}
		else
		{
			scale = width_x;
		}
	}

	// Calculate normals
	for (unsigned y = 1; y < width_y - 1; ++y)
	{
		for (unsigned x = 1; x < width_x - 1; ++x)
		{
			float s01 = getHeight(x - 1, y) * scale;
			float s21 = getHeight(x + 1, y) * scale;
			float s10 = getHeight(x, y - 1) * scale;
			float s12 = getHeight(x, y + 1) * scale;

			// Get tangents in the x and y directions
			Vector3 vx(2.0f, 0, s21 - s01);
			Vector3 vy(0, 2.0f, s12 - s10);

			// Calculate the cross product of the two normals
			vx = normalize(vx);
			vy = normalize(vy);
			normal.setVector(x, y, cross(vx, vy));
			tangent.setVector(x, y, vx);
		}
	}

	// Calculate normals on the edges
	for (unsigned x = 1; x < width_x - 1; ++x)
	{
		// Top edge
		float s01 = getHeight(x - 1, 0) * scale;
		float s21 = getHeight(x + 1, 0) * scale;
		float s10 = getHeight(x, 0) * scale;
		float s12 = getHeight(x, 1) * scale;

		Vector3 vx(2.0f, 0, s21 - s01);
		Vector3 vy(0, 2.0f, s12 - s10);

		vx = normalize(vx);
		vy = normalize(vy);
		normal.setVector(x, 0, cross(vx, vy));
		tangent.setVector(x, 0, vx);

		// Bottom edge
		unsigned y = width_y - 1;
		s01 = getHeight(x - 1, y) * scale;
		s21 = getHeight(x + 1, y) * scale;
		s10 = getHeight(x, y - 1) * scale;
		s12 = getHeight(x, y) * scale;

		vx = { 2.0f, 0, s21 - s01 };
		vy = { 0, 2.0f, s12 - s10 };

		vx = normalize(vx);
		vy = normalize(vy);
		normal.setVector(x, y, cross(vx, vy));
		tangent.setVector(x, y, vx);
	}

	for (unsigned y = 1; y < width_y - 1; ++y)
	{
		// Left edge
		float s01 = getHeight(0, y) * scale;
		float s21 = getHeight(1, y) * scale;
		float s10 = getHeight(0, y - 1) * scale;
		float s12 = getHeight(0, y + 1) * scale;

		Vector3 vx(2.0f, 0, s21 - s01);
		Vector3 vy(0, 2.0f, s12 - s10);

		vx = normalize(vx);
		vy = normalize(vy);
		normal.setVector(0, y, cross(vx, vy));
		tangent.setVector(0, y, vx);

		// Right edge
		unsigned x = width_x - 1;
		s01 = getHeight(x - 1, y) * scale;
		s21 = getHeight(x, y) * scale;
		s10 = getHeight(x, y - 1) * scale;
		s12 = getHeight(x, y + 1) * scale;

		vx = { 2.0f, 0, s21 - s01 };
		vy = { 0, 2.0f, s12 - s10 };

		vx = normalize(vx);
		vy = normalize(vy);
		normal.setVector(x, y, cross(vx, vy));
		tangent.setVector(x, y, vx);
	}

	// Get the corners last

	// Top left corner
	unsigned x = 0;
	unsigned y = 0;
	float s01 = getHeight(x, y) * scale;
	float s21 = getHeight(x + 1, y) * scale;
	float s10 = getHeight(x, y) * scale;
	float s12 = getHeight(x, y + 1) * scale;

	Vector3 vx(2.0f, 0, s21 - s01);
	Vector3 vy(0, 2.0f, s12 - s10);

	vx = normalize(vx);
	vy = normalize(vy);
	normal.setVector(x, y, cross(vx, vy));
	tangent.setVector(x, y, vx);

	// Top right corner
	x = width_x - 1;
	y = 0;
	s01 = getHeight(x - 1, y) * scale;
	s21 = getHeight(x, y) * scale;
	s10 = getHeight(x, y) * scale;
	s12 = getHeight(x, y + 1) * scale;

	vx = { 2.0f, 0, s21 - s01 };
	vy = { 0, 2.0f, s12 - s10 };

	vx = normalize(vx);
	vy = normalize(vy);
	normal.setVector(x, y, cross(vx, vy));
	tangent.setVector(x, y, vx);

	// Bottom left corner
	x = 0;
	y = width_y - 1;
	s01 = getHeight(x, y) * scale;
	s21 = getHeight(x + 1, y) * scale;
	s10 = getHeight(x, y - 1) * scale;
	s12 = getHeight(x, y) * scale;

	vx = { 2.0f, 0, s21 - s01 };
	vy = { 0, 2.0f, s12 - s10 };

	vx = normalize(vx);
	vy = normalize(vy);
	normal.setVector(x, y, cross(vx, vy));
	tangent.setVector(x, y, vx);

	// Bottom right corner
	x = width_x - 1;
	y = width_y - 1;
	s01 = getHeight(x - 1, y) * scale;
	s21 = getHeight(x, y) * scale;
	s10 = getHeight(x, y - 1) * scale;
	s12 = getHeight(x, y) * scale;

	vx = { 2.0f, 0, s21 - s01 };
	vy = { 0, 2.0f, s12 - s10 };

	vx = normalize(vx);
	vy = normalize(vy);
	normal.setVector(x, y, cross(vx, vy));
	tangent.setVector(x, y, vx);
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