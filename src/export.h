#pragma once

#include <string>

typedef unsigned char byte;

// A buffer used to store pixel data for exporting as a greyscale png image
class PixelBuffer
{
public:
	PixelBuffer(unsigned _width, unsigned _height, unsigned _size);
	~PixelBuffer();

	unsigned getWidth() const;
	unsigned getHeight() const;
	unsigned getSize() const;

	// Fill a pixel with a 16 bit integer
	// (THROWS runtime_error if the pixel size is not 16 bits)
	void fillPixel(unsigned x, unsigned y, uint16_t value);
	// Fill a pixel with an 8 bit integer
	// (THROWS runtime_error if the pixel size is not 8 bits)
	void fillPixel(unsigned x, unsigned y, uint8_t value);

	// Export the pixel buffer as a greyscale PNG
	// (THROWS exception when an error occurs with the file saving)
	void save(std::string filename);

private:
	unsigned width;		// The width of each row in pixels
	unsigned height;	// The number of rows of pixels in the buffer
	unsigned size;		// The size, in bytes, of each pixel

	byte** data = nullptr;
};