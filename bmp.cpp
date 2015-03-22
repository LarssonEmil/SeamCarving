using namespace std;

#include "bmp.h"

bmp::~bmp()
{
	delete[] data;
}

void bmp::fill() 
{	
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

	std::string imagepath = "image4.bmp";
	FILE* file = fopen(imagepath.c_str(), "rb");
	if (!file)
		exit(0);

	if (fread(header, 1, 54, file) != 54) // If not 54 bytes read : problem
	{

	}

	if (header[0] != 'B' || header[1] != 'M')
	{

	}

	// Read ints from the byte array
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)
		imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)
		dataPos = 54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	//Everything is in memory now, the file can be closed
	fclose(file);

}

