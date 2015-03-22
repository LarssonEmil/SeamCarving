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
		unsigned int weight, computeWeight;
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
	int RemoveNext(Node* _this);
	enum Next
	{
		LEFT,
		MIDLE,
		RIGHT,
		SELF
	};
public:
	LinkSquare();
	~LinkSquare();
	void RemoveSeam(Node* _this);
	Node* LowStreamStart(int* startDrawat);
	Node* root;
	Node* bottom;
	int xsize;
	int ysize;
	int xStart;
	int yStart;
	void LoadImageData();
	void CalcGradient();
	void computeWeights();
};


#endif