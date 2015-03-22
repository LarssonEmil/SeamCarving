#include "LinkSquare.h"

LinkSquare::LinkSquare()
{

}

LinkSquare::~LinkSquare()
{
	if (root)
	{
		Node* traverse = root;
		Node* down = root;

		for (int y = 0; y < ysize; y++)
		{
			traverse = down;
			if (y + 1 != ysize)
				down = down->down;
			else
			{
				bottom;
				int test = 3;
			}

			for (int x = 0; x < xsize; x++)
			{
				traverse = traverse->right;
				delete traverse->left;
				if (x + 1 == xsize)
					delete traverse;
			}
		}
	}
}

void LinkSquare::LoadImageData()
{
	bmp file;
	file.fill();
	xStart = xsize = file.width;
	yStart = ysize = file.height;

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
	
	//last row pointer
	bottom = traverse;

	for (;;) // set all down pointers in last row to null
	{
		traverse->down = 0;
		if (traverse->right)
			traverse = traverse->right;
		else
			break;
	}

	//only need to set once so I do it here
	bottom->computeWeight = 99999999;
	root->computeWeight = 99999999;
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

void LinkSquare::RemoveSeam(Node* _this)
{
	int next = RemoveNext(_this);

	//red mark
	if (_this->left)
		_this->left->deleteFlag = 1;

	if (next == LEFT)
	{
		_this->left->down = _this->down;
		_this->down->up = _this->left;

		//link sides
		if (_this->right)
			_this->right->left = _this->left;
		if (_this->left)
			_this->left->right = _this->right;

		RemoveSeam(_this->down->left);
		delete _this;
		return;
	}
	else if (next == RIGHT)
	{
		_this->right->down = _this->down;
		_this->down->up = _this->right;

		//link sides
		if (_this->right)
			_this->right->left = _this->left;
		if (_this->left)
			_this->left->right = _this->right;

		RemoveSeam(_this->down->right);
		delete _this;
		return;
	}
	
	//link sides
	if (_this->right)
		_this->right->left = _this->left;
	if (_this->left)
		_this->left->right = _this->right;

	if (next == SELF)
	{
		delete _this;
		return;
	}
	//midel
	RemoveSeam(_this->down);
	delete _this;
	return;
}

LinkSquare::Node* LinkSquare::LowStreamStart(int* startDrawAt)
{
	Node* lowest = root->right;
	Node* traverse = lowest;
	*startDrawAt = 0;
	for (int n = 3; n < xsize; n++)
	{
		traverse = traverse->right;
		if (traverse->computeWeight < lowest->computeWeight)
		{
			*startDrawAt = n;
			lowest = traverse;
		}
	}
	return lowest;
}

int LinkSquare::RemoveNext(Node* _this)
{
	if (_this->down)
	{
		if (_this->right)
		{
			if (_this->down->computeWeight > _this->right->computeWeight)
			{
				if (_this->left)
				{
					if (_this->right->computeWeight > _this->left->computeWeight)
						return LEFT;
				}
				return RIGHT;
			}
		}
		if (_this->left)
		{
			if (_this->down->computeWeight > _this->left->computeWeight)
				return LEFT;
		}
		return MIDLE;
	}
	else
	{
		return SELF;
	}
}

void LinkSquare::CalcGradient()
{
	Node* traverse = root;
	Node* traverseRight = root;

	for (int y = 0; y < ysize; y++)
	{
		for (int x = 0; x < xsize; x++)
		{
			gradientMagniture(traverseRight);
			traverseRight = traverseRight->right;
		}
		traverse = traverse->down;
		traverseRight = traverse;
	}
}

void LinkSquare::gradientMagniture(Node* _this)
{
	{
		float dx;
		if (_this->left && _this->right)
			dx = luminosity(_this->left) - luminosity(_this->right);
		else
			dx = 999;

		float dy;
		if (_this->up && _this->down)
			dy = luminosity(_this->up) - luminosity(_this->down);
		else
			dy = 999;

		_this->weight = sqrt((dx * dx) + (dy * dy)) * 0.7;
	}
}

float LinkSquare::luminosity(Node* _this)
{
		return (0.2126f * _this->red +
			0.7152f * _this->green +
			0.0722 * _this->blue);
}

void LinkSquare::computeWeights()
{
	Node* traverse = bottom;
	Node* traverseRight = bottom;

	//compute weights for bottom row
	for (int x = 0; x < xsize; x++)
	{
		traverseRight->computeWeight = traverseRight->weight;
		traverseRight = traverseRight->right;
	}
	traverseRight = bottom;
	//compute weights for rest
	for (int y = 0; y < ysize-1; y++)
	{
		traverse = traverse->up;
		traverseRight = traverse;

		for (int x = 0; x < xsize; x++)
		{
			//if edges not null
			if (traverseRight->left && traverseRight->right)
			{
				//find cheapest path
				int minWeight = traverseRight->down->left->computeWeight;
				if (minWeight > traverseRight->down->computeWeight)
					minWeight = traverseRight->down->computeWeight;
				if (minWeight > traverseRight->down->right->computeWeight)
					minWeight = traverseRight->down->right->computeWeight;

				traverseRight->computeWeight = traverseRight->weight + minWeight;
			}
			else // weight of edge nodes
				traverseRight->computeWeight = 999999999;

				traverseRight = traverseRight->right;
		}
	}
}