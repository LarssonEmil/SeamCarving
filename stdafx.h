#pragma once

#include <windows.h>
#include <D3D10.h>
#include <d3dcompiler.h>
//#include <D3DX10.h>

#pragma comment(lib, "D3D10.lib")
#pragma comment(lib, "D3dcompiler.lib")

#include <string>
#include <fstream>
#include <vector>

#define SAFE_RELEASE(x) if( x ) { (x)->Release(); (x) = nullptr; }
#define SAFE_DELETE(x) if( x ) { delete(x); (x) = nullptr; }
#define PI (3.14159265358979323846f)

const unsigned int SCREEN_WIDTH	= 1024;
const unsigned int SCREEN_HEIGHT	= 768;