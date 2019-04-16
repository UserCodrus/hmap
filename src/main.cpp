#include <iostream>
#include <string>

#include <png.h>

#include "heightmap.h"
#include "export.h"

using namespace std;

int main(int argc, char** argv)
{
	string fname = "heightmap.png";	// The default filename
	unsigned width = 128;			// Heightmap dimensions
	unsigned height = 128;

	// Get the command line arguments
	if (argc > 1)
	{
		int i = 1;

		// Get the file name if the first argument is not a flag
		if (argv[1][0] != '-' && argv[1][0] != '/')
		{
			fname = argv[1];
			i++;
		}

		// Get flags from the command line
		for (i; i < argc; ++i)
		{
			if (argv[i][0] == '-' || argv[i][0] == '/')
			{
				switch (argv[i][1])
				{
				case 'w':
					// Get the width of the heightmap
					if (argc > i + 1)
					{
						try
						{
							width = stoi(argv[++i]);
						}
						catch (invalid_argument e)
						{
							cout << "Heightmap dimensions are invalid";
							return 0;
						}
					}
					break;

				case 'h':
					// Get the height of the heightmap
					if (argc > i + 1)
					{
						try
						{
							height = stoi(argv[++i]);
						}
						catch (invalid_argument e)
						{
							cout << "Heightmap dimensions are invalid";
							return 0;
						}
					}
					break;
				}
			}
		}
	}

	if (width == 0 || height == 0)
	{
		cout << "Heightmap dimensions are invalid";
		return 0;
	}

	// Create the heightmap
	Heightmap<uint16_t> map(width, height);
	for (unsigned y = 0; y < width; ++y)
	{
		for (unsigned x = 0; x < height; ++x)
		{
			uint16_t delta = (uint16_t)U16::MAXIMUM;
			uint16_t minimum = (uint16_t)U16::MINIMUM;
			map.setData(x, y, minimum + x * ( delta / width));
		}
	}

	// Load height data into a byte buffer
	PixelBuffer image(map.getWidthX(), map.getWidthY(), map.getSize());
	for (unsigned y = 0; y < map.getWidthY(); ++y)
	{
		for (unsigned x = 0; x < map.getWidthX(); ++x)
		{
			image.fillPixel(x, y, map.getData(x, y));
		}
	}

	// Save the heightmap as a png
	try
	{
		image.save(fname);

		cout << "Heightmap exported to " << fname << endl;
	}
	catch (exception& e)
	{
		cout << "Export failed:\n" << e.what() << endl;
	}

	return 0;
}