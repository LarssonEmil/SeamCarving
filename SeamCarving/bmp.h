#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>



class bmp
{
public:
	void fill();
	~bmp();
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3
	// Actual RGB data
	unsigned char * data = 0;
};