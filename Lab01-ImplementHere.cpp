//--------------------------------------------------------------------------------------
// File: Lab01-ImplementHere.cpp
//
// DV1212 Lab01: RayTracing
//
// Copyright (c) Stefan Petersson 2010. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "ToScreen.h"
extern ToScreen* g_ToScreen;

//simple math vector-class
struct Vec
{
	float	x, y, z;

	Vec() {};
	Vec(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	float Length()
	{
		return sqrt(x*x + y*y + z*z);
	}

	void Normalize()
	{
		float l = Length();
		x /= l;
		y /= l;
		z /= l;
	}
};

struct Ray
{
	Vec	o;
	Vec	d;

	Ray(Vec origin, Vec direction)
	{
		o = origin;
		d = direction;
	}
};

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

void Update(float deltaTime)
{
	//loop through every screen pixel from top to botttom
	for(unsigned int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for(unsigned int x = 0; x < SCREEN_WIDTH; x++)
		{
			g_ToScreen->SetPixelColor(x, y, 255, 255, x+y);
		}
	}

	for (unsigned int x = 0; x < 150; x++)
		for (unsigned int y = 0; y < 20; y++)
			g_ToScreen->SetPixelColor(x, y, 0, 0, 0);


	//update DX10-surface with new pixel data
	g_ToScreen->Update(deltaTime);
}