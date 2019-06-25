#include "export.h"

#include <stdexcept>
#include <cstdio>
#include <png.h>

PixelBuffer::PixelBuffer(unsigned _width, unsigned _height, unsigned _size)
{
	if (_width == 0 || _height == 0 || _size == 0)
	{
		throw std::bad_alloc();
	}

	width = _width;
	height = _height;
	size = _size;

	// Create pixel row pointers
	data = new byte* [height];

	// Create the pixel rows
	for (unsigned y = 0; y < height; ++y)
	{
		data[y] = new byte[width * size];

		for (unsigned x = 0; x < width; ++x)
		{
			for (unsigned b = 0; b < size; ++b)
			{
				data[y][x * size + b] = 0;
			}
		}
	}
}

PixelBuffer::~PixelBuffer()
{
	for (unsigned i = 0; i < height; ++i)
	{
		if (data[i] != nullptr)
		{
			delete[] data[i];
		}
	}
	if (data != nullptr)
	{
		delete[] data;
	}
}

unsigned PixelBuffer::getWidth() const
{
	return width;
}

unsigned PixelBuffer::getHeight() const
{
	return height;
}

unsigned PixelBuffer::getSize() const
{
	return size;
}

void PixelBuffer::fillPixel(unsigned x, unsigned y, uint16_t value)
{
	// Check for a size mismatch between the pixel size and a uint16
	if (size > sizeof(uint16_t))
	{
		throw std::overflow_error("Pixel buffer is too large to accomodate a 16 bit integer");
	}
	else if (size < sizeof(uint16_t))
	{
		throw std::underflow_error("Pixel buffer is too small to accomodate a 16 bit integer");
	}

	// Add the data to the pixel buffer one byte at a time
#ifndef BIGENDIAN
	data[y][x * size] = (value >> 8) & 0xFF;
	data[y][x * size + 1] = value & 0xFF;
#else
	data[y][x * size] = value & 0xFF;
	data[y][x * size + 1] = (value >> 8) & 0xFF;
#endif
}

void PixelBuffer::fillPixel(unsigned x, unsigned y, uint8_t value)
{
	// Check for a size mismatch between the pixel size and a uint8
	if (size > sizeof(uint8_t))
	{
		throw std::overflow_error("Pixel buffer is too large to accomodate an 8 bit integer");
	}

	// Add the data to the pixel buffer one byte at a time
	data[y][x * size] = value;
}

void PixelBuffer::save(std::string filename)
{
	// Open the file
	FILE* file;
	if (fopen_s(&file, filename.c_str(), "wb"))
	{
		std::string message = "Unable to create file " + filename + "\nPlease ensure that the file name is valid";
		throw std::exception(message.c_str());
	}

	// Create write and info structures
	png_struct* png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr)
	{
		fclose(file);
		throw std::exception("Unable to initialize png writer");
	}

	png_info* info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fclose(file);
		png_destroy_write_struct(&png_ptr, nullptr);
		throw std::exception("Unable to initialize png data");
	}

	// Setup jumpbuf for png errors
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(file);
		png_destroy_write_struct(&png_ptr, nullptr);
		throw std::exception("Unable to write png file");
	}

	// Add header data
	png_set_IHDR(
		png_ptr, info_ptr,
		width,							// Image dimensions
		height,
		size * 8,						// Bit depth
		PNG_COLOR_TYPE_GRAY,			// Color channels
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT
	);

	// Write image data
	png_init_io(png_ptr, file);
	png_set_rows(png_ptr, info_ptr, data);
	png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(file);
}