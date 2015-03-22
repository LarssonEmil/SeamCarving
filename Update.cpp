
#include "stdafx.h"
#include "ToScreen.h"
#include "LinkSquare.h"
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

void Draw(LinkSquare*, bool, bool);
void Draw2(LinkSquare* , bool , bool , LinkSquare::Node , int );
void DrawLastBlack(LinkSquare*);

bool init = false;
bool draw = true;
bool lastGradient = false;
bool lastshowRed = false;
LinkSquare::Node* updateFrom;
LinkSquare::Node* todel;

void Update(LinkSquare* LSQ, float deltaTime, bool deletion, bool gradient, bool showRed)
{
	if (!init)
	{
		init = true;
		//LSQ->LoadImageData();
		LSQ->CalcGradient();
		LSQ->computeWeights();
		Draw(LSQ, gradient, showRed);
	}
	if (deletion)
	{
		if (LSQ->xsize > 5)
		{
			int startDraw = 0;
			//LSQ->CalcGradient();
			LSQ->computeWeights();
			todel = LSQ->LowStreamStart(&startDraw);
			updateFrom = todel->left;
			LSQ->RemoveSeam(todel);
			DrawLastBlack(LSQ);
			LSQ->xsize--;
			
			Draw2(LSQ, gradient, showRed, *updateFrom, startDraw -1);
			g_ToScreen->Update(deltaTime);
		}
	}
	else
	{
		Draw(LSQ, gradient, showRed);
		//update DX10-surface with new pixel data
		g_ToScreen->Update(deltaTime);
	}
}

void Draw(LinkSquare* LSQ, bool gradient, bool showRed)
{
	LinkSquare::Node* traverse = LSQ->root;
	LinkSquare::Node* start = traverse;

	for (int y = 1; y < LSQ->ysize; y++)
	{
		for (int x = 0; x < LSQ->xsize; x++)
		{
			if (showRed)
				if (traverse->deleteFlag > 0)
					g_ToScreen->SetPixelColor(x, y, 255, 0, 0);
				else
				{
					if (!gradient)
						g_ToScreen->SetPixelColor(x, y, traverse->red, traverse->green, traverse->blue);
					else
						g_ToScreen->SetPixelColor(x, y, traverse->weight, traverse->weight, traverse->weight);
				}
			else
			{
				if (!gradient)
					g_ToScreen->SetPixelColor(x, y, traverse->red, traverse->green, traverse->blue);
				else
					g_ToScreen->SetPixelColor(x, y, traverse->weight, traverse->weight, traverse->weight);
			}
			traverse = traverse->right;
		}

		start = start->down;
		traverse = start;
	}
}

void Draw2(LinkSquare* LSQ, bool gradient, bool showRed, LinkSquare::Node start1, int xpos)
{
	LinkSquare::Node* traverse = &start1;
	LinkSquare::Node* start = traverse;

	for (int y = 1; y < LSQ->ysize; y++)
	{
		for (int x = xpos; x < LSQ->xsize; x++)
		{
			if (showRed)
				if (traverse->deleteFlag > 0)
					g_ToScreen->SetPixelColor(x, y, 255, 0, 0);
				else
				{
					if (!gradient)
						g_ToScreen->SetPixelColor(x, y, traverse->red, traverse->green, traverse->blue);
					else
						g_ToScreen->SetPixelColor(x, y, traverse->weight, traverse->weight, traverse->weight);
				}
			else
			{
				if (!gradient)
					g_ToScreen->SetPixelColor(x, y, traverse->red, traverse->green, traverse->blue);
				else
					g_ToScreen->SetPixelColor(x, y, traverse->weight, traverse->weight, traverse->weight);
			}
			traverse = traverse->right;
		}
		
		if (xpos > 1)
		{
			start = start->down->left;
			traverse = start;
			xpos--;
		}
		else
		{
			start = start->down;
			traverse = start;
		}
	}
}

void DrawLastBlack(LinkSquare* LSQ)
{
	for (int n = 0; n < LSQ->ysize; n++)
	{
		g_ToScreen->SetPixelColor(LSQ->xsize, n, 0,0,0);
	}
}