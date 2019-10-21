#pragma once
#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_ //we define these to be used in the system class
#define WIN32_LEAN_AND_MEAN //to speed up the build process//

#include "inputclass.h"
#include "graphicsclass.h"
#include <windows.h>

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();   
	void Shutdown();   //functions that are called in the main.cpp
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM); //function to handle the windows system messages

private:
	bool Frame();
	void InitializeWindows(int&, int&); //this isnt exaplained in the tutorial
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;			//neither is this
	HWND m_hwnd;

	InputClass* m_Input;           //pointers to the two objects that handle graphics and input 
	GraphicsClass* m_Graphics;
};



static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 


static SystemClass* ApplicationHandle = 0;   //included so we can re-direct the windows system messaging into our 'MessageHandler' function



#endif