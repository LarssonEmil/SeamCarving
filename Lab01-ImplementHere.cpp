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

	float Dot(Vec& v2)
	{
		return (x * v2.x) + (y * v2.y) + (z * v2.z);
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

struct Plane
{
	Vec		n;
	float	d;
	Color	c;

	Plane(Vec normal, float _d, Color color)
	{
		n = normal;
		d = _d;
		c = color;
	}
};

struct HitData
{
	float	t;
	Color	color;

	HitData()
	{
		t = -1.0f;
	}
};

// full details: see intersection lecture slides
void RayVsPlane(Ray& r, Plane& p, HitData& hitData)
{
	float t = (p.d - r.o.Dot(p.n)) / r.d.Dot(p.n);

	if(t > 0.0f)
	{
		hitData.t = t;
		hitData.color = p.c;
	}
	else
	{
		hitData.t = -1.0f;
	}
}

void Update(float deltaTime)
{
	//hitdata is sent to intersection tests to return intersection info
	HitData hitData;


	//create plane
	static float col = 0;
	col += deltaTime * 100;
	Plane p(Vec(0,0,(BYTE)-1), 1, Color(0,0,(BYTE)col));

	//loop through every screen pixel from top to botttom
	for(unsigned int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for(unsigned int x = 0; x < SCREEN_WIDTH; x++)
		{
			//create ray for current pixel
			Ray r(Vec((float)x, (float)y, -10), Vec(0,0,1));
			
			//check if ray intersect the plane
			RayVsPlane(r, p, hitData);

			//check if ray did hit the plane
			if(hitData.t > 0.0f)
			{
				//if a hit then set color at current pixel position
				g_ToScreen->SetPixelColor(
					x,
					y,
					hitData.color.r,
					hitData.color.g,
					hitData.color.b
					);
			}
		}
	}

	//update DX10-surface with new pixel data
	g_ToScreen->Update(deltaTime);
}