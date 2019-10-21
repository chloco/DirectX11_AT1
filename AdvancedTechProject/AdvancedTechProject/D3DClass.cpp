#include "d3dclass.h"

D3DClass::D3DClass()
{
	m_swapChain = 0;		//pointers from the header file initialized to null
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::D3DClass(const D3DClass& other)
{
}


D3DClass::~D3DClass()
{
}

//does the entire setup of Direct3D for DirectX 11. theres extra stuff here for future development
//screenwidth and height variables that are given to this function from the systemclass. DirectX uses these to initialze window dimensions
//HWND variable is a handle to the window. DirectX need this handle to access the window previously created.
//fullscreen variable is whether we are running in windowed mode or fullscreen. Direct3D needs this for initializing with the correct settings
//ScreeNear variables are the depth settings for our 3D environment that will be rendered in the window. The vsync variable indicates if we want Direct3D to render
//according to the users' monitor refresh rate/go as fast as possible
bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen,  
	float screenDepth, float screenNear)															
{																									
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;


	// Store the vsync setting.
	m_vsync_enabled = vsync;

	///WE NEED TO GET THE REFRESH RATE FROM THE VIDEO CARD/MONITOR BEFORE WE INITIALIZE DIRECT3D. EACH COMPUTER MAY BE DIFFERENT
	///SO WE NEED TO QUERY FOR THAT INFO. WE QUERY FOR THE NUMERATOR AND DENOMINATOR VALUES AND THEN PASS THEM ONTO DIRECTX DURING SETUP & IT WILL CALCULATE THE REFRESH RATE
	///IF WE DIDNT DO THIS: DIRECTX WILL RESPOND BY PERFORMING A BLIT INSTEAD OF A BUFFER FLIP = DEGRADE PERFORMANCE/GIVE ANNOYING ERRORS IN THE OUTPUT

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (i = 0; i<numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	///NOW WE HAVE THE NUMERATOR AND DENOMINATOR FOR THE REFRESH RATE. USING THE ADAPTER WE RETRIEVE THE NAME OF THE VIDEO CARD AND THE AMOUNT OF VIDEO MEMORY///

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	///Now that we have stored the numerator and denominator for the refresh rate and the video card information we can release the structures and interfaces used to get that information.//

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	///NOW WE HAVE THE REFRESH RATE, WE CAN START THE DIRECTX INITIALIZATION, NEXT WE WILL FILL OUT THE DESCRIPTION OF THE SWAP CHAIN.
	///THE SWAP CHAIN IS THE FRONT AND BACK BUFFER TO WHICH THE GRAPHICS WILL BE DRAWN. (YOUSE A SINGLE BACK BUFFER, DO ALL MY DRAWINGS TO IT, THEN
	///SWAP TO THE FRONT BUFFER WHICH THEN DISPLAYED ON THE SCREEN) (WHICH IS WHY ITS CALLED A SWAP CHAIN)

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	///NEXT PART OF THE DESCIPTION IS THE REFRESH RATE, (HOW MANY TIMES A SECOND IT DRAWS THE BACK BUFFER TO THE FRONT BUFFER)
	///IF VSYNC IS TRUE IN OUR GRAPHICS..H HEADER, THEN THIS WILL LOCK THE REFRESH RATE TO THE SYSTEM SETTINGS (60HZ)
	///IF VSYNC IS FALSE, IT WILL DRAW THE SCREEN AS MANY TIMES AS IT CAN, CAN CAUSE SOME VISUAL ARTIFACTS.

	// Set the refresh rate of the back buffer.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	///NOW WE HAVE THE SWAP CHAIN DESCRIPTION, WE NEED TO SET UP A VARIABLE CALLED FEATURE LEVEL. THIS VARIABLE TELLS DIRECTX WHAT VERSION WE PLAN TO USE (11)

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	///NOW WE HAVE ALL THE INFO WE NEED, WE CAN NOW CREATE THE SWAP CHAIN, THE DIRECT3D DEVICE AND THE DIRECT3D DECVICE CONTEXT
	///(WHICH ARE VERY IMPORTANT, THE INTERFACE TO ALL OF THE DIRECT3D FUNCTIONS) (IT WILL BE USED FOR ALMOST EVERYTHING FROM THIS POINT)

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;		//IF THE USER DOESNT HAVE DIRECTX 11, THEN THE PROGRAM WILL FAIL
	}

	///Sometimes this call to create the device will fail if the primary video card is not compatible with DirectX 11. Some machines may have the primary card as a 
	///DirectX 10 video card and the secondary card as a DirectX 11 video card. Also some hybrid graphics cards work that way with the primary being the low power Intel 
	///card and the secondary being the high power Nvidia card. To get around this you will need to not use the default device and instead enumerate all the video cards in 
	///the machine and have the user choose which one to use and then specify that card when creating the device.

	//Now we have swap chain we need to get a pointer to the back buffer and attach it to the swap chain. CreateRenderTargetVIew function will be used to attach it

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	///NOW WE SET UP A DEPTH BUFFER DESCRIPTION. DEPTH BUFFER IS USED SO OUR POLYGONS CAN BE RENDERED PROPERLY IN 3D SPACE. WE WILL ALSO ATTACH A STENCIL BUFFER TO OUR DEPTH BUFFER
	///THIS IS USED TO ACHIEVE EFFECTS SUCH AS MOTION BLUR, VOLUMETRIC SHADOWS ETC.

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	///NOW WE CREATE THE DEPTH/STENCIL BUFFER USING THESE DESCRIPTIONS. CREATETEXTURE2D FUNCTION TO MAKE BUFFERS, HENCE THE BUFFER IS JUST A 2D TEXTURE.
	///ONCE YOUR POLYGONS ARE SORTED AND RASTERIZED THEY END UP BEING COLOURED PIXELS IN THIS 2D BUFFER, THEN THIS 2D BUFFER IS DRAWN TO THE SCREEN

	// Create the texture for the depth buffer using the filled out description.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	///NOW SETUP THE DEPTH STENCIL DESCRIPTION, ALLOWS US TO CONTROL WHAT TYPE OF DEPTH TEST DIRECT3D WILL DO FOR EACH PIXEL

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	///WE NOW CREATE A DEPTH STENCIL STATE

	// Create the depth stencil state.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	///NOW WE SET IT SO THE DEPTH STENCIL STATE WILL TAKE EFFECT. WE USE DEVICE CONTEXT TO SET IT

	// Set the depth stencil state.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	///NEXT WE CREATE IS THE DESCRIPTION OF THE DEPTH STENCIL BUFFER. WE DO THIS SO DIRECT3D KNOWS TO USE THE DEPTH BUFFER AS A STENCIL TEXTURE.
	///WE THEN CALL CREATE DEPTH STENCIL VIEW TO CREATE IT

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	///WE NOW CALL OMSETRENDERTARGET. THIS BINDS THE RENDER TARGET VIEW AND THE DEPTH STENCIL TO THE OUTPUT RENDER PIPELINE. THIS WAY GRAPHICS THAT THE PIPELINE
	///RENDERS WILL GET DRAWN TO OUR BACK BUFFER THAT WE ALREADY CREATED. WITH THE GRAPHICS WRITTEN TO THE BACK BUFFER, WE CAN THEN SWAP IT TO THE FRONT DISPLAY OUR GRAPHICS ON
	///THE USERS SCREEN

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	//NOW RENDER TARGETS ARE SETUP IM SETTING UP FUNCTIONS FOR MORE CONTROL IN FUTURE DEVELOPMENT
	///FIRST THING: RASTERIXER STATE, WILL GIVE US CONTROL OVER HOW POLYGONS ARE RENDERED, DO THINGS LIKE RENDER IN WIREFRAME OR HAVE DIRECTX DRAW BOTH THE FRONT AND BACK OF POLYGONS.
	///BY DEFAULT, DIRECTX ALREADY HAS A RASTERIZER STATE SET UP AND WORKING, EXCEPT THIS WAY YOU HAVE CONTROL OVER CHANGING IT

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// Now set the rasterizer state.
	m_deviceContext->RSSetState(m_rasterState);

	///VIEW PORT NEEDS TO BE SETUP SO THAT DIRECT3D CAN MAP CLIP SPACE COORDINATES TO THE RENDER TARGET SPACE. (IE. THE ENTIRE WINDOW)

	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &viewport);

	///SETTING UP PROJECTION MATRIX USED TO TRANSLATE THE 3D SCENE IN THE 2D VIEWPORT SPACE THAT WE PREVIOUSLY CREATED. WE NEED A COPY OF THIS MATRIX SO WE CAN
	///PASS IT TO OUR SHADERS, THAT WILL BE USED FOR RENDERING OUR SCENES

	// Setup the projection matrix.
	fieldOfView = 3.141592654f / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// Create the projection matrix for 3D rendering.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	///CREATING ANOTHER MATRIX CALLED THE WORLD MATRIX. USED TO CONVERT THE VERTICES OF OUR OBJECT INTO VERTICES IN THE 3D SCENE. CAN ALSO BE USED TO ROTATE, TRANSLATE AND SCALE OUR OBJECTS IN 3D SPACE.
	
	// Initialize the world matrix to the identity matrix.
	m_worldMatrix = XMMatrixIdentity();

	///NOW SETTING UP THE ORTHOGRAPHIC PROJECTION MATRIX. USED FOR RENDERING 2D ELEMENTS LIKE USER INTERFACES ON THE SCREEN.

	// Create an orthographic projection matrix for 2D rendering.
	m_orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

///WE NOW MAKE THE SHUTDOWN FUNCTION WHICH WILL RELEASE AND SHUTDOWN ALL THE POINTERS USED IN THE INITALIZE FUNCTION. WE FORCE WINDOWED MODE TO AVOID THROWING ANY EXCEPTIONS

void D3DClass::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}

///BEGIN SCENE AND ENDSCENE, CALLED FOR WHENEVER IM GOING TO DRAW A NEW 3D SCENE AT THE BEGINNING OF EACH FRAME. INITIALIZES BUFFERS SO THEY ARE BLANK AND READY TO BE DRAWN TO 

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

///TELLS THE SWAP CHAIN TO DISPLAY OUR 3D SCENE ONCE ALL OF THE DRAWING HAS COMPLETED AT THE END OF EACH FRAME.
void D3DClass::EndScene()
{
	// Present the back buffer to the screen since rendering is complete.
	if (m_vsync_enabled)
	{
		// Lock to screen refresh rate.
		m_swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		m_swapChain->Present(0, 0);
	}

	return;
}

///GET POINTERS TO THE DIRECT3D DEVICE CONTEXT. THESE HELPER FUNCTIONS WILL BE CALLED BY THE FRAMEWORK OFTEN 

ID3D11Device* D3DClass::GetDevice()
{
	return m_device;
}


ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_deviceContext;
}

///3 HELPER FUNCTIONS TO GIVE COPIES OF THE PROJECTION, WORLD AND ORTHOGRAPHIC MATRICES TO CALLING FUNCTIONS. MOST SHADERS WILL NEED THESE MATRICES FOR RENDERING

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
	return;
}


void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

//REFERENCES THE NAME OF THE VIDEO CARD AND AMOUNT OF VIDEO MEMORY

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}
