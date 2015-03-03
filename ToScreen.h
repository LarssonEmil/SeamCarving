#ifndef _TO_SCREEN__H
#define _TO_SCREEN__H

//#include <D3DX10.h>
#include "stdafx.h"

struct TriVertex
{
	float pos[3];
};

class ToScreen
{
public:
	ToScreen();
	~ToScreen();

	void Release();
	HRESULT Init(HWND hwnd);
	
	HRESULT Update(float deltaTime);
	HRESULT Render();
	HRESULT SetPixelColor(unsigned int x, unsigned int y, BYTE r, BYTE g, BYTE b);

private:

	D3D10_DRIVER_TYPE       mDriverType;
	IDXGISwapChain*         mSwapChain;
	ID3D10RenderTargetView* mRenderTargetView;
	ID3D10Texture2D*        mDepthStencil;
	ID3D10DepthStencilView* mDepthStencilView;
	ID3D10Device*			mDevice;

	ID3D10Texture2D*		mTexture;
	BYTE*					mScreenPixels;

	ID3D10Resource* mCurrentFrame;
	ID3D10ShaderResourceView* mCurrentFrameView;

	//shader variables
	ID3D10VertexShader*		mVertexShader;
	ID3D10PixelShader*		mPixelShader;
	ID3D10SamplerState*		mTextureSampler;

	unsigned int mScreenWidth;
	unsigned int mScreenHeight;

private:
	HRESULT CompileShader(char* shaderFile, char* pEntrypoint, char* pTarget, D3D10_SHADER_MACRO* pDefines, ID3DBlob** pCompiledShader);
	HRESULT CreateShadersAndInputLayout();
};

#endif