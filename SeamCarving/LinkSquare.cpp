#include "LinkSquare.h"

LinkSquare::LinkSquare()
{

}

void LinkSquare::LoadImageData()
{
	bmp file;
	file.fill();
	xsize = file.width;
	ysize = file.height;

	populate();

	insert(&file);
}

void LinkSquare::populate()
{
	root = new Node();
	nodeLink(0, root, 0, xsize);
	root->red = 123;
	Node* traverse = root;
	for (int n = 0; n < ysize - 1; n++)
	{
		traverse->down = new Node();
		Node* tempUp = traverse;
		traverse = traverse->down;
		nodeLink(0, traverse, tempUp, xsize);
		if (n + 1 < ysize)
			traverse = tempUp->down;
		else
			traverse = tempUp;
	}

	for (;;) // set all down pointers in last row to null
	{
		traverse->down = 0;
		if (traverse->right)
			traverse = traverse->right;
		else
			break;
	}
}

void LinkSquare::nodeLink(Node* last, Node* _this, Node* top, int x)
{
	_this->left = last;
	_this->up = top;
	if (!top) //if top is null (first row case)
	{
		if (x > 0) //check last collumn case
		{
			_this->right = new Node();
			nodeLink(_this, _this->right, 0, --x);
		}
		else
			_this->right = 0;
	}
	else
	{
		top->down = _this;
		if (x > 0)
		{
			_this->right = new Node();
			nodeLink(_this, _this->right, top->right, --x);
		}
		else
			_this->right = 0;
	}
}

void LinkSquare::insert(bmp* bmpfile)
{
	//BGR
	Node* traverse = root;
	Node* traverseRight = root;

	for (int y = bmpfile->height - 1; y > 0; y--)
	{
		if (y != 0)
		{
			traverse = traverse->down;
			traverseRight = traverse;
		}

		for (int x = 0; x < bmpfile->width; x++)
		{
			traverseRight->blue = bmpfile->data[0 + (x * 3) + (y * bmpfile->width * 3) ];
			traverseRight->green = bmpfile->data[1 + (x * 3) + (y * bmpfile->width * 3) ];
			traverseRight->red = bmpfile->data[2 + (x * 3) + (y * bmpfile->width * 3) ];
			if (x + 1 < bmpfile->width)
				traverseRight = traverseRight->right;
		}
	}
}

void LinkSquare::CalcGradient()
{
	Node* traverse = root;
	Node* traverseRight = root;

	for (int y = 0; y < ysize; y++)
	{
		if (y != 0)
		{
			traverse = traverse->down;
			traverseRight = traverse;
		}

		for (int x = 0; x < xsize; x++)
		{
			luminosity(traverseRight);
			if (x + 1 < xsize)
				traverseRight = traverseRight->right;
		}
	}
}

void LinkSquare::gradientMagniture(Node* _this)
{

}

float LinkSquare::luminosity(Node* _this)
{

}