#include"graphicsclass.h"

GraphicsClass::GraphicsClass() //constructor
{
	m_Direct3D = 0; //initialize to null for safety reasons
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
	m_CatModel = 0;
	m_Input = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other) //deconstructor
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, InputClass* input)
{
	bool result;


	// Create the Direct3D object.
	m_Direct3D = new D3DClass; //create the D 3D object
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR); //sending the screen width, height, handle to the window and 4 global variables from the graphics header file. the 
	if (!result)																											 //D3D will use this information to setup the Direct 3D system
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	//m_Camera->SetPosition(-2.0f, -0.5f, -5.0f);
	//m_Camera->SetRotation(0.0f, 30.0f ,0.0f);
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);
	m_Camera->SetRotation(0.0f, 0.0f, 0.0f);

	// Create the model object.
	m_Model = new ModelClass;
	m_CatModel = new ModelClass;
	if (!m_Model)
	{
		return false;
	}
	if (!m_CatModel)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "happyfur.tga","cat2.txt.");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	result = m_CatModel->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), "treadmill.tga", "treadmill.txt.");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the cat model object.", L"Error", MB_OK);
		return false;
	}

	//free hong kong
	// Create the color shader object.
// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	m_Input = input;

	return true;
}


void GraphicsClass::Shutdown() //shutdown of all the graphics objects occur here so we have placed the D3D shutdown in THIS function. 
{
	
	//check to see if the pointer was initialized or not, it it wasnt we can assume it was never set up and not try to shut it down.
	//thus why its important to set all pointers to null; if it does find the pointer has been initialized then it will attempt to shut down the D3D class and clean up the pointer afterwards


// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_CatModel)
	{
		m_CatModel->Shutdown();
		delete m_CatModel;
		m_CatModel = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}


bool GraphicsClass::Frame() //calls the render function in each frame
{

	XMFLOAT3 pos = m_Camera->GetPosition();
	XMFLOAT3 rot = m_Camera->GetRotation();
	
	//W MOVES UP
	if (m_Input->IsKeyDown(0x57) == true && m_Input->IsKeyDown(0x10))
	{
		pos.y += 0.05;
		m_Camera->SetPosition(pos.x,pos.y,pos.z);
	}

	///W AND SHIFT MOVES FORWARD
	else if (m_Input->IsKeyDown(0x57) == true)
	{
		pos.z += 0.05;
		m_Camera->SetPosition(pos.x, pos.y, pos.z);
	
	}
	//S MOVES DOWN
	if (m_Input->IsKeyDown(0x53) == true && m_Input->IsKeyDown(0x10))
	{
		pos.y -= 0.05;
		m_Camera->SetPosition(pos.x, pos.y, pos.z);
	}
	//S AND SHIFT MOVES BACKWARDS
	else if (m_Input->IsKeyDown(0x53) == true)
	{
		pos.z -= 0.05;
		m_Camera->SetPosition(pos.x, pos.y, pos.z);

	}
	//A MOVES LEFT
	if (m_Input->IsKeyDown('A') == true)
	{
		pos.x -= 0.05;
		m_Camera->SetPosition(pos.x, pos.y, pos.z);
	}

	//D MOVES RIGHT
	if (m_Input->IsKeyDown(0x44) == true)
	{
		pos.x += 0.05;
		m_Camera->SetPosition(pos.x, pos.y, pos.z);
	}


	//Q ROTATES CAM
	if (m_Input->IsKeyDown(0x51) == true)
	{
		rot.y -= 1;
		m_Camera->SetRotation(rot.x, rot.y, rot.z);
	}

	//E ROTATES CAM
	if (m_Input->IsKeyDown(0x45) == true)
	{
		rot.y += 1;
		m_Camera->SetRotation(rot.x, rot.y, rot.z);
	}

	bool result;
	
	// Render the graphics scene.
	result = Render();
	if (!result)	//check if render is returned correctly
	{
		return false;
	}
	return true;
}


bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(1.0f, 0.0f, 0.5f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the texture shader.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetVertexCount(), m_Model->GetInstanceCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_Model->GetTexture());
	if (!result)
	{
		return false;
	}

	m_CatModel->Render(m_Direct3D->GetDeviceContext());

	// Render the model using the treadmill texture shader.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_CatModel->GetVertexCount(), m_CatModel->GetInstanceCount(), worldMatrix, viewMatrix,
		projectionMatrix, m_CatModel->GetTexture());
	if (!result)
	{
		return false;
	}
	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}