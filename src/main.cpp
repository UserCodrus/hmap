#include "heightmap.h"
#include "export.h"

#include <iostream>
#include <string>
#include <random>
#include <chrono>

#include <png.h>

using namespace std;
typedef std::chrono::steady_clock Timer;

int main(int argc, char** argv)
{
	string fname = "heightmap.png";	// The filename the png will be saved to
	unsigned width = 1024;			// Heightmap width
	unsigned height = 1024;			// Heightmap height
	float min_height = 0.0f;		// Minimum map height
	float max_height = 1.0f;		// Maximum map height
	
	// Get the start time
	auto t_start = Timer::now();
	// Generate the default rng seed
	unsigned seed = (unsigned)t_start.time_since_epoch().count();

	// Get the command line arguments
	if (argc > 1)
	{
		int i = 1;

		// Get the file name from the first parameter if it is not a flag
		if (argv[1][0] != '-' && argv[1][0] != '/')
		{
			fname = argv[1];
			i++;
		}

		// Read other command line parameters
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

				case 's':
					// Get the rng seed
					if (argc > i + 1)
					{
						try
						{
							seed = stoi(argv[++i]);
						}
						catch (invalid_argument e)
						{
							cout << "Invalid seed value";
							return 0;
						}
					}
					break;

				case 't':
					// Get the maximum height
					if (argc > i + 1)
					{
						try
						{
							max_height = stof(argv[++i]);
						}
						catch (invalid_argument e)
						{
							cout << "Invalid height value";
							return 0;
						}
					}
					break;

				case 'b':
					// Get the minimum height
					if (argc > i + 1)
					{
						try
						{
							min_height = stof(argv[++i]);
						}
						catch (invalid_argument e)
						{
							cout << "Invalid height value";
							return 0;
						}
					}
					break;
				}
			}
		}
	}

	// Display selected parameters
	cout << "Seed value: " << seed << endl;
	cout << "Width: " << width << ", Height: " << height << endl;
	cout << "Upper bound: " << max_height << ", Lower bound: " << min_height << endl;

	if (width == 0 || height == 0 || max_height > 1.0f || min_height < 0.0f)
	{
		cout << "Heightmap dimensions are invalid";
		return 0;
	}

	// Create the heightmap
	cout << "\nGenerating heightmap... ";
	Heightmap map(width, height);

	//map.perlinOctaves(map, seed, min_height, max_height, 16, 4, 0.6f);
	//map.perlinNotch(seed, min_height, max_height, 3, 12, 0.6f);
	map.random(seed, min_height, max_height, 16);
	//map.diamondSquare(seed, min_height, max_height, 8);

	// Measure the time taken to create the heightmap
	auto t_gen = Timer::now();
	chrono::duration<double> delta = t_gen - t_start;
	cout << delta.count() << "s";

	// Load height data into a byte buffer
	cout << "\nExporting heightmap... ";
	PixelBuffer image(map.getWidthX(), map.getWidthY(), map.getSize());
	for (unsigned y = 0; y < map.getWidthY(); ++y)
	{
		for (unsigned x = 0; x < map.getWidthX(); ++x)
		{
			image.fillPixel(x, y, map.getHeight(x, y));
		}
	}

	// Save the heightmap as a png
	try
	{
		image.save(fname);

		// Measure the time taken to package the heightmap
		auto t_pack = Timer::now();
		chrono::duration<double> delta = t_pack - t_gen;
		cout << delta.count() << "s";

		cout << "\n\nHeightmap saved to " << fname << endl;
	}
	catch (exception& e)
	{
		cout << "\n\nExport failed:\n" << e.what() << endl;
	}

	return 0;
}