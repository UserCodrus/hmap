template <class T>
void Heightmap::set(const T& noise, float (T::* sample)(float, float) const, float scale)
{
	// Modify the frequency of the noise so that samples are within the bounds of the noise
	float fx = noise.scaleWidth(width_x);
	float fy = noise.scaleHeight(width_y);

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] = (noise.*sample)(x * fx, y * fy) * scale;
		}
	}
}

template <class T>
void Heightmap::add(const T& noise, float (T::* sample)(float, float) const, float scale)
{
	// Modify the frequency of the noise so that samples are within the bounds of the noise
	float fx = noise.scaleWidth(width_x);
	float fy = noise.scaleHeight(width_y);

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] += (noise.*sample)(x * fx, y * fy) * scale;
		}
	}
}

template <class T>
void Heightmap::multiply(const T& noise, float (T::* sample)(float, float) const, float scale)
{
	// Modify the frequency of the noise so that samples are within the bounds of the noise
	float fx = noise.scaleWidth(width_x);
	float fy = noise.scaleHeight(width_y);

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] *= (noise.*sample)(x * fx, y * fy) * scale;
		}
	}
}