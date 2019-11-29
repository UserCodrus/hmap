#include "generate.h"
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

	string generator_name;			// The name of the generator being used
	vector<float> generator_data;	// Data for the heightmap generator

	bool gen_normals = false;		// Set to true to generate normals for the heightmap
	
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
				case 'W':
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

				case 'H':
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

				case 'S':
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

				case 'T':
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

				case 'B':
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

				case 'G':
				case 'g':
					// Get the name of the generator
					if (argc > i + 1)
					{
						++i;
						generator_name = argv[i];

						// Get data input for the generator
						if (argc > i + 1)
						{
							++i;
							try
							{
								while (true)
								{
									// Add the data
									generator_data.push_back(stof(argv[i]));

									// Move to the next string
									if (argc > i + 1)
									{
										if (argv[i + 1][0] == '-' || argv[i + 1][0] == '/')
										{
											// Next string is a switch
											break;
										}
										else
										{
											++i;
										}
									}
									else
									{
										// Out of strings
										break;
									}
								}
							}
							catch (invalid_argument e)
							{
								cout << "Invalid height value";
								return 0;
							}
						}
					}
					break;

				case 'N':
				case 'n':
					gen_normals = true;
					break;
				}
			}
		}
	}

	// Display selected parameters
	cout << "Seed value: " << seed << endl;
	cout << "Width: " << width << ", Height: " << height << endl;
	cout << "Upper bound: " << max_height << ", Lower bound: " << min_height << endl;
	cout << "Generator: ";
	if (!generator_name.empty())
	{
		cout << generator_name;
		for (unsigned i = 0; i < generator_data.size(); ++i)
		{
			cout << " " << generator_data[i];
		}
	}
	else
	{
		cout << "default";
	}
	cout << endl;

	// Check parameters
	if (width == 0 || height == 0 || max_height > 1.0f || min_height < -1.0f || min_height >= max_height)
	{
		cout << "Heightmap dimensions are invalid";
		return 0;
	}

	// Create the heightmap
	cout << "\nGenerating heightmap... ";
	t_start = Timer::now();
	Heightmap map(width, height);

	bool done = false;
	if (!generator_name.empty())
	{
		// Use the specified generator
		if (generator_name == "random" || generator_name == "Random")
		{
			if (generator_data.size() > 2)
			{
				MapGenerator::layeredWhiteNoise(map, seed, min_height, max_height, (unsigned)generator_data[0], (unsigned)generator_data[1], generator_data[2]);
				done = true;
			}
		}
		else if (generator_name == "plasma" || generator_name == "Plasma")
		{
			if (generator_data.size() > 0)
			{
				MapGenerator::plasma(map, seed, min_height, max_height, (unsigned)generator_data[0]);
				done = true;
			}
		}
		else if (generator_name == "perlin" || generator_name == "Perlin")
		{
			if (generator_data.size() > 2)
			{
				MapGenerator::layeredPerlin(map, seed, min_height, max_height, (unsigned)generator_data[0], (unsigned)generator_data[1], generator_data[2]);
				done = true;
			}
		}
	}

	if (!done)
	{
		MapGenerator::defaultGenerator(map, seed, min_height, max_height);
	}

	// Measure the time taken to create the heightmap
	auto t_now = Timer::now();
	chrono::duration<double> delta = t_now - t_start;
	cout << delta.count() << "s";

	// Generate normals and tangents
	if (gen_normals)
	{
		cout << "\nCalculating normals... ";
		t_start = Timer::now();
		Vectormap normals, tangents;
		map.calculateNormals(normals, tangents);

		// Measure the time taken to generate normals
		t_now = Timer::now();
		delta = t_now - t_start;
		cout << delta.count() << "s";
	}

	// Load height data into a byte buffer
	cout << "\nExporting heightmap... ";
	t_start = Timer::now();
	PixelBuffer image(map.getWidthX(), map.getWidthY(), sizeof(uint16_t));
	for (unsigned y = 0; y < map.getWidthY(); ++y)
	{
		for (unsigned x = 0; x < map.getWidthX(); ++x)
		{
			// Convert noise to a 16 bit integer and write it to the image
			image.fillPixel(x, y, (uint16_t)((map.getHeight(x, y) * 0.5f + 0.5f) * std::numeric_limits<uint16_t>::max()));
		}
	}

	// Save the heightmap as a png
	try
	{
		image.save(fname);

		// Measure the time taken to package the heightmap
		t_now = Timer::now();
		delta = t_now - t_start;
		cout << delta.count() << "s";

		cout << "\n\nHeightmap saved to " << fname << endl;
	}
	catch (exception& e)
	{
		cout << "\n\nExport failed:\n" << e.what() << endl;
	}

	return 0;
}