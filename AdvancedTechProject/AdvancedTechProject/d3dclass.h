#pragma once
#include <Windows.h>
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

//Linking
#pragma comment(lib, "d3d11.lib") //Direct3D functionality for setting up and drawing 3D graphics in DirectX 11
#pragma comment(lib, "dxgi.lib")  //Contains tools to interface with the hardware on your computer to obtain info about the refresh rate of the monitor, the video card being used etc.
#pragma comment(lib, "d3dcompiler.lib") //contains functionality for compiling shaders

//Includes
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);		//constructor and deconstructor 
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float); //initializing functions we need
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&); //for camera use in the future
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void GetVideoCardInfo(char*, int&);

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;
};
#endif