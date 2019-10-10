template <class T>
void Heightmap::sample(T& noise, float (T::* sample)(float, float) const, float scale)
{
	// Scale the noise
	noise.scale(width_x, width_y);

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			data[y * width_x + x] = (noise.*sample)((float)x, (float)y) * scale;
		}
	}
}