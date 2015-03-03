//--------------------------------------------------------------------------------------
// File: ToScreen.cpp
//
// DV1212 Lab01: RayTracing
//
// Copyright (c) Stefan Petersson 2010. All rights reserved.
//--------------------------------------------------------------------------------------
#include "ToScreen.h"		
#include "stdafx.h"

char* SHADER_STRING = R"(
	struct PSSceneIn
	{
		float4 Pos : SV_Position;
		float2 TexCoords : TEX;
	};

	SamplerState samLinear : register(s0);
	Texture2D frame : register(t0);

	//-----------------------------------------------------------------------------------------
	// VertexShader: VSScene
	//-----------------------------------------------------------------------------------------
	PSSceneIn VS_main(uint vertexId : SV_VertexID)
	{
		PSSceneIn output = (PSSceneIn)0;

		if (vertexId == 3)
			output.Pos = float4(1.0, 1.0, 0.5, 1.0);
		else if (vertexId == 2)
			output.Pos = float4(1.0, -1.0, 0.5, 1.0);
		else if (vertexId == 1)
			output.Pos = float4(-1.0, 1.0, 0.5, 1.0);
		else if (vertexId == 0)
			output.Pos = float4(-1.0, -1.0, 0.5, 1.0);

		output.TexCoords = float2(0.5f, 0.5f) * output.Pos.xy + 0.5f.xx;

		return output;
	}

	//-----------------------------------------------------------------------------------------
	// PixelShader: PSSceneMain
	//-----------------------------------------------------------------------------------------
	float4 PS_main(PSSceneIn input) : SV_Target
	{
		return frame.Sample(samLinear, input.TexCoords);
	}
	)"; //end of raw string
ToScreen::ToScreen()
{
	mCurrentFrame = nullptr;
	mCurrentFrameView = nullptr;

	mDriverType			= D3D10_DRIVER_TYPE_NULL;
	mSwapChain			= nullptr;
	mRenderTargetView	= nullptr;
	mDepthStencil		= nullptr;
	mDepthStencilView	= nullptr;
	mDevice				= nullptr;
	mScreenPixels		= nullptr;
	mTexture			= nullptr;

	mVertexShader = nullptr;
	mPixelShader = nullptr;
	mTextureSampler = nullptr;
}

ToScreen::~ToScreen()
{
	Release();
}

HRESULT ToScreen::CompileShader(char* shaderText, char* pEntrypoint, char* pTarget, D3D10_SHADER_MACRO* pDefines, ID3DBlob** pCompiledShader)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_IEEE_STRICTNESS;

	ID3DBlob* pErrorBlob = NULL;
	HRESULT hr = D3DCompile(
		shaderText,
		strlen(shaderText),
		NULL,
		pDefines,
		NULL,
		pEntrypoint,
		pTarget,
		dwShaderFlags,
		NULL,
		pCompiledShader,
		&pErrorBlob);

	if (pErrorBlob)
	{
		OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		SAFE_RELEASE(pErrorBlob);
	}

	return hr;
}

HRESULT ToScreen::CreateShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	ID3DBlob* pVertexShader = NULL;
	if (SUCCEEDED(hr = CompileShader(SHADER_STRING, "VS_main", "vs_4_0", NULL, &pVertexShader)))
	{
		if (SUCCEEDED(hr = mDevice->CreateVertexShader(
			pVertexShader->GetBufferPointer(),
			pVertexShader->GetBufferSize(),
			&mVertexShader)))
		{

		}

		SAFE_RELEASE(pVertexShader);
	}

	ID3DBlob* pPixelShader = NULL;
	if (SUCCEEDED(hr = CompileShader(SHADER_STRING, "PS_main", "ps_4_0", NULL, &pPixelShader)))
	{
		hr = mDevice->CreatePixelShader(
			pPixelShader->GetBufferPointer(),
			pPixelShader->GetBufferSize(),
			&mPixelShader);

		SAFE_RELEASE(pPixelShader);
	}

	return hr;
}

HRESULT ToScreen::Init(HWND hwnd)
{
	HRESULT hr = S_OK;;

	RECT rc;
	GetClientRect( hwnd, &rc );
	mScreenWidth = rc.right - rc.left;;
	mScreenHeight = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
#endif

	D3D10_DRIVER_TYPE driverTypes[] = 
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = mScreenWidth;
	sd.BufferDesc.Height = mScreenHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		mDriverType = driverTypes[driverTypeIndex];
		hr = D3D10CreateDeviceAndSwapChain( nullptr, mDriverType, nullptr, createDeviceFlags, 
			D3D10_SDK_VERSION, &sd, &mSwapChain, &mDevice );
		if( SUCCEEDED( hr ) )
			break;
	}
	if( FAILED(hr) )
		return hr;

	// Create a render target view
	ID3D10Texture2D* pBackBuffer;
	hr = mSwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	hr = mDevice->CreateRenderTargetView( pBackBuffer, nullptr, &mRenderTargetView );
	pBackBuffer->Release();
	if( FAILED(hr) )
		return hr;


	// Create depth stencil texture
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = mScreenWidth;
	descDepth.Height = mScreenHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mDevice->CreateTexture2D( &descDepth, nullptr, &mDepthStencil );
	if( FAILED(hr) )
		return hr;

	// Create the depth stencil view
	D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mDevice->CreateDepthStencilView( mDepthStencil, &descDSV, &mDepthStencilView );
	if( FAILED(hr) )
		return hr;


	mDevice->OMSetRenderTargets( 1, &mRenderTargetView, mDepthStencilView );

	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = mScreenWidth;
	vp.Height = mScreenHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	mDevice->RSSetViewports( 1, &vp );



	if (FAILED(CreateShadersAndInputLayout()))
	{
		MessageBoxA(0, "Shader compilation error", "Shader error!", 0);
	}

	mScreenPixels = new BYTE[mScreenWidth * mScreenHeight * 4];

	for(unsigned int y = 0; y < mScreenHeight; y++)
		for(unsigned int x = 0; x < mScreenWidth; x++)
			SetPixelColor(x, y, 0,0,0);

	return S_OK;
}

void ToScreen::Release()
{
	if(mScreenPixels)
		delete [] mScreenPixels;

	SAFE_RELEASE(mCurrentFrameView);

	if(mCurrentFrame != nullptr)
		mCurrentFrame->Release();
	mCurrentFrame = nullptr;

	SAFE_RELEASE(mTexture);
	SAFE_RELEASE(mDepthStencil);

	SAFE_RELEASE(mTextureSampler);
	SAFE_RELEASE(mVertexShader);
	SAFE_RELEASE(mPixelShader);

	SAFE_RELEASE(mRenderTargetView);
	SAFE_RELEASE(mDepthStencilView);
	SAFE_RELEASE(mSwapChain);
	SAFE_RELEASE(mDevice);

	if (mDevice) mDevice->ClearState();
}

HRESULT ToScreen::Update(float deltaTime)
{
	if(mCurrentFrame != nullptr)
		mCurrentFrame->Release();

	//lazy teachers (SPR and FLL) were here :-)
	if (!mTextureSampler)
	{
		D3D10_SAMPLER_DESC desc;
		memset(&desc, 0, sizeof(desc));
		desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D10_COMPARISON_NEVER;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;

		mDevice->CreateSamplerState(&desc, &mTextureSampler);
	}

	if(!mTexture)
	{
		D3D10_SUBRESOURCE_DATA data;
		data.pSysMem = mScreenPixels;
		data.SysMemPitch = (mScreenWidth * 4);
		data.SysMemSlicePitch = 0;

		D3D10_TEXTURE2D_DESC TextureDesc;
		ZeroMemory( &TextureDesc, sizeof( TextureDesc ) );
		TextureDesc.Width = mScreenWidth;
		TextureDesc.Height = mScreenHeight;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		TextureDesc.SampleDesc.Count = 1;
		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.Usage = D3D10_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;
		mDevice->CreateTexture2D( &TextureDesc, &data, &mTexture );
	
		D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = TextureDesc.Format;
		srvDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = TextureDesc.MipLevels;

		if(mCurrentFrameView)
			mCurrentFrameView->Release();

		mDevice->CreateShaderResourceView( mTexture, &srvDesc, &mCurrentFrameView );
		//Texture->Release();
	}
	else
	{
		// update texture resource with new pixel data
		D3D10_BOX destRegion;
		destRegion.left = 0;
		destRegion.right = mScreenWidth;
		destRegion.top = 0;
		destRegion.bottom = mScreenHeight;
		destRegion.front = 0;
		destRegion.back = 1;

		mDevice->UpdateSubresource(mTexture, 0, &destRegion, mScreenPixels, mScreenWidth * 4, 0);
	}

	return S_OK;
}

HRESULT ToScreen::Render()
{
	// Set variables
	//mEffect->GetVariableByName( "frame" )->AsShaderResource()->SetResource(mCurrentFrameView);
	mDevice->PSSetShaderResources(0, 1, &mCurrentFrameView);
	mDevice->PSSetSamplers(0, 1, &mTextureSampler);

	// Set Input Assembler params
	//UINT stride = sizeof(TriVertex);
	//UINT offset = 0;
	mDevice->IASetInputLayout( nullptr );
	mDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
	//mDevice->IASetVertexBuffers( 0, 0, nullptr, nullptr, nullptr );

	static float ClearColor[4] = { 0.5f, 0.5f, 0.5f, 0.0f };
	
	//clear render target
	mDevice->ClearRenderTargetView( mRenderTargetView, ClearColor );

	//clear depth info
	mDevice->ClearDepthStencilView( mDepthStencilView, D3D10_CLEAR_DEPTH, 1.0f, 0 );

	// Render line using the technique mRenderTextured
	mDevice->VSSetShader(mVertexShader);
	mDevice->GSSetShader(nullptr);
	mDevice->PSSetShader(mPixelShader);
	mDevice->Draw(4, 0);

	if(FAILED(mSwapChain->Present( 0, 0 )))
		return E_FAIL;

	return S_OK;
}

HRESULT ToScreen::SetPixelColor(unsigned int x, unsigned int y, BYTE r, BYTE g, BYTE b)
{
	if(x < mScreenWidth && y < mScreenHeight)
	{
		unsigned char* t1 = &mScreenPixels[((mScreenHeight - y - 1) * mScreenWidth + x) * 4];
		
		t1[0] = r;
		t1[1] = g;
		t1[2] = b;
		t1[3] = 255;

		return S_OK;
	}

	return E_FAIL;
}