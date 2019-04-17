template <typename hdata>
Heightmap<typename hdata>::Heightmap(unsigned size_x, unsigned size_y)
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

template <typename hdata>
Heightmap<typename hdata>::~Heightmap()
{
	if (data != nullptr)
	{
		delete[] data;
	}
}

template <typename hdata>
unsigned Heightmap<typename hdata>::getWidthX() const
{
	return width_x;
}

template <typename hdata>
unsigned Heightmap<typename hdata>::getWidthY() const
{
	return width_y;
}

template <typename hdata>
unsigned Heightmap<typename hdata>::getSize() const
{
	return sizeof(hdata);
}

template <typename hdata>
hdata Heightmap<typename hdata>::getData(unsigned x, unsigned y) const
{
	if (x >= width_x || y >= width_y)
	{
		throw new std::exception("Array out of bounds in heightmap");
	}

	return data[y * width_x + x];
}

template <typename hdata>
void Heightmap<typename hdata>::setData(unsigned x, unsigned y, hdata value)
{
	if (x >= width_x || y >= width_y)
	{
		throw new std::exception("Array out of bounds in heightmap");
	}

	data[y * width_x + x] = value;
}