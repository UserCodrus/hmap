#pragma once

template <class T>
void Heightmap::noiseAdd(const T& noise, float (T::* sample)(float, float) const, float scale)
{
	// Modify the frequency of the noise so that samples are within the bounds of the noise
	float fx = (float)(noise.getWidth() - 1) / width_x;
	float fy = (float)(noise.getHeight() - 1) / width_y;

	// Apply noise
	for (unsigned y = 0; y < width_y; ++y)
	{
		for (unsigned x = 0; x < width_x; ++x)
		{
			setHeight(x, y, (noise.*sample)(x * fx, y * fy) * scale);
		}
	}
}