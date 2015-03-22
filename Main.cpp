//--------------------------------------------------------------------------------------
// File: Lab01-Main.cpp
//
// DV1212 Lab01: RayTracing
//
// Copyright (c) Stefan Petersson 2009. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "ToScreen.h"
#include "LinkSquare.h"


//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE				g_hInst					= nullptr;  
HWND					g_hWnd					= nullptr;

ToScreen*				g_ToScreen				= nullptr;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT				Render();
void				Update(LinkSquare* lsq, float deltaTime, bool deletion, bool gradient, bool showRed);

LinkSquare LSQ;
unsigned int SCREEN_WIDTH;
unsigned int SCREEN_HEIGHT;
//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Init()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	g_ToScreen = new ToScreen();
	g_ToScreen->Init(g_hWnd);

	return S_OK;

}

HRESULT Render()
{
	g_ToScreen->Render();

	return S_OK;
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	LSQ.LoadImageData();
	SCREEN_WIDTH = LSQ.xsize;
	SCREEN_HEIGHT = LSQ.ysize;

	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

	if( FAILED( Init() ) )
		return 0;

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	bool Remove = false;
	bool Gradient = false;
	bool Red = false;

	bool lastGradient = true;
	bool lastRed = false;

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
			
			switch (msg.message)
			{
			case WM_KEYDOWN:
				WPARAM param = msg.wParam;
				char c = MapVirtualKey(param, MAPVK_VK_TO_CHAR);
				if (c == 'Q')
				{
					if (Remove)
						Remove = false;
					else
						Remove = true;
				}
				else if (c == 'W')
				{
					if (Gradient)
						Gradient = false;
					else
						Gradient = true;
				}
				else if (c == 'E')
				{
					if (Red)
						Red = false;
					else
						Red = true;
				}

			}
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			//need to updatecheck
		
			//render
			if(Remove || Gradient != lastGradient || Red != lastRed)
				Update(&LSQ ,dt, Remove, Gradient, Red);
			lastGradient = Gradient;
			lastRed = Red;
			//Remove = false;
			//Sleep(0);
			Render();

			prevTimeStamp = currTimeStamp;
		}
	}

	SAFE_DELETE(g_ToScreen);

	return (int) msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = nullptr;
	wcex.lpszClassName  = L"Seam Carving";
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if(!(g_hWnd = CreateWindow(
							L"Seam Carving",
							L"Seam Carving",
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							nullptr,
							nullptr,
							hInstance,
							nullptr)))
	{
		return E_FAIL;
	}

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:

		switch(wParam)
		{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}