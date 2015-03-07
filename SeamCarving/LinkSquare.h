#ifndef LINK_SQUARE_H
#define LINK_SQUARE_H

#include <iostream>
#include <fstream>
#include <vector>
#include "bmp.h"

typedef unsigned char BYTE;

class LinkSquare
{
public:
	struct Node
	{
		BYTE red, green, blue, deleteFlag;
		float weight, computeWeight;
		Node* up;
		Node* right;
		Node* down;
		Node* left;
		Node()
		{
			deleteFlag = 0;
		}
	};
private:
	void populate();
	void nodeLink(Node* last, Node* _this, Node* top, int x);
	void insert(bmp* data);
	void gradientMagniture(Node*);
	float luminosity(Node*);
public:
	LinkSquare();
	Node* root;
	int xsize;
	int ysize;
	void LoadImageData();
	void CalcGradient();
};


#endif