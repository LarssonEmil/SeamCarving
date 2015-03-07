
#include "stdafx.h"
#include "ToScreen.h"
#include "SeamCarving\LinkSquare.h"
extern ToScreen* g_ToScreen;

struct Color
{
	BYTE	r;
	BYTE	g;
	BYTE	b;

	Color() { memset(&r, 0, 3); }
	Color(BYTE red, BYTE green, BYTE blue)
	{
		r = red;
		g = green;
		b = blue;
	}
};

void Draw(LinkSquare*, bool, bool, int);

bool init = true;


void Update(LinkSquare* LSQ, float deltaTime, bool deletion, bool gradient, bool showRed)
{
	int x = 0;
	if (deletion)
	{
		//calc magnitudes
		//calc seams
		//delete (&x)
	}

	Draw(LSQ, gradient, showRed, x);

	//update DX10-surface with new pixel data
	g_ToScreen->Update(deltaTime);
}

void Draw(LinkSquare* LSQ, bool gradient, bool showRed, int updateFromX)
{
	if (gradient)
	{

	}
	else
	{
		LinkSquare::Node* traverse = LSQ->root;
		LinkSquare::Node* traverseRight = LSQ->root;

		for (int k = 0; k < LSQ->ysize; k++)
		{
			if (k != 0)
			{
				traverse = traverse->down;
				traverseRight = traverse;
			}

			for (int n = 0; n < LSQ->xsize; n++)
			{
				g_ToScreen->SetPixelColor(n, k, traverseRight->red, traverseRight->green, traverseRight->blue);

				if (n + 1 < LSQ->xsize)
					traverseRight = traverseRight->right;
			}
		}
	}
}