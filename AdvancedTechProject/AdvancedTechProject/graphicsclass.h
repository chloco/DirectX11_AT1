#pragma once
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_ 
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "colorshaderclass.h"
#include "textureshaderclass.h"
#include "graphicsclass.h"
#include "inputclass.h"

//Global Variables that we may want to change, e.g. windowed or full screen mode
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0F;
const float SCREEN_NEAR = 0.1f;

//The other object that is created by the system class (input is the other). All the graphics functionality in this application will be encapsulated in this class.
//this class will contain the graphics objects
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	//ColorShaderClass* m_ColorShader;
	TextureShaderClass* m_TextureShader;
	InputClass* m_Input;

};
#endif