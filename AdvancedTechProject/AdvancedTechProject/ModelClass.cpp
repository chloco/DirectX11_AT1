////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	//m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
	m_instanceBuffer = 0;
	xPos = 0;
	yPos = 0;
	zPos = 0;
	xScale = 1;
	yScale = 1;
	zScale = 1;

}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename, char* modelFilename)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, deviceContext, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Shutdown()
{
	//release the model texture
	ReleaseTexture();
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();
// Release the model data.
	ReleaseModel();
	return;
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetVertexCount()
{
	return m_vertexCount;
}


int ModelClass::GetInstanceCount()
{
	return m_instanceCount;
}

int ModelClass::GetCatInstanceCount()
{
	return m_instanceCountCats;
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

XMFLOAT3 ModelClass::getPosition()
{
	XMFLOAT3 position = {xPos, yPos, zPos};
	return position;
}

XMFLOAT3 ModelClass::getScale()
{
	XMFLOAT3 scale = { xScale, yScale, zScale };
	return scale;
}

//XMMATRIX ModelClass::Scale
//{
//	
//	FLOAT ScaleX;    // x=axis scale
//	FLOAT ScaleY;     // y-axis scale
//	FLOAT ScaleZ;    // z-axis scale
//
//}

void ModelClass::setPosition(float x, float y, float z)
{
	xPos = x;
	yPos = y;
	zPos = z;
}

void ModelClass::setScale(float x, float y, float z)
{
	xScale = x;
	yScale = y;
	zScale = z;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	InstanceType* instances;
	InstanceType* cat;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	//unsigned long* indices;
	//D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	//D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	//HERE WE SET THE VERTICES OF THE OBJECT!!!!
	// Set the number of vertices in the vertex array.
	//m_vertexCount = 3;

	//// Set the number of indices in the index array.
	//m_indexCount = 3;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	//// Create the index array.
	//indices = new unsigned long[m_indexCount];
	//if (!indices)
	//{
	//	return false;
	//}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3((m_model[i].x + xPos)* xScale, (m_model[i].y + yPos) * yScale, (m_model[i].z + zPos) * zScale);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		//indices[i] = i;
	}


	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	//indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	//indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//indexBufferDesc.CPUAccessFlags = 0;
	//indexBufferDesc.MiscFlags = 0;
	//indexBufferDesc.StructureByteStride = 0;

	//// Give the subresource structure a pointer to the index data.
	//indexData.pSysMem = indices;
	//indexData.SysMemPitch = 0;
	//indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	//result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	//delete[] indices;
	//indices = 0;
	// Set the number of instances in the array.
	m_instanceCount = 3;
	m_instanceCountCats = 3;


	// Create the instance array.
	instances = new InstanceType[m_instanceCount];
	if (!instances)
	{
		return false;
	}

	/* Create the instance array.*/
	/*cat = new InstanceType[m_instanceCountCats];
	if (!cat)
	{
		return false;
	}*/

	// Load the instance array with data.
	instances[0].position = XMFLOAT3(-1.5f, -1.5f, 5.0f);
	instances[1].position = XMFLOAT3(-8.0f, -1.5f, 5.0f);
	instances[2].position = XMFLOAT3(-15.0f, -1.5f, 5.0f);
	/*cat[0].position = XMFLOAT3(-1.5f, -1.5f, 10.0f);
	cat[1].position = XMFLOAT3(-8.0f, -6.5f, 5.0f);
	cat[2].position = XMFLOAT3(-7.0f, -10.5f, 5.0f);*/

	/*instances[1].position = XMFLOAT3(5.0f, -1.5f, 5.0f);
	instances[2].position = XMFLOAT3(1.5f, -1.5f, 5.0f);
	instances[3].position = XMFLOAT3(1.5f, 1.5f, 5.0f);*/


	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	

	// Give the subresource structure a pointer to the instance data.
	
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//instanceCatDesc.Usage = D3D11_USAGE_DEFAULT;
	//instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCountCats;
	//instanceCatDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//instanceCatDesc.CPUAccessFlags = 0;
	//instanceCatDesc.MiscFlags = 0;
	//instanceCatDesc.StructureByteStride = 0;

	//instanceCatData.SysMemPitch = 0;
	//instanceCatData.SysMemSlicePitch = 0;
	//instanceData.pSysMem = cat;

	//result = device->CreateBuffer(&instanceCatDesc, &instanceCatData, &m_instanceCatBuffer);
	//if (FAILED(result))
	//{
	//	return false;
	//}

	// Release the instance array now that the instance buffer has been created and loaded.
	delete[] instances;
	instances = 0;

	/*delete[] cat;
	cat = 0;*/
	return true;
}


void ModelClass::ShutdownBuffers()
{
	//// Release the index buffer.
	//if (m_indexBuffer)
	//{
	//	m_indexBuffer->Release();
	//	m_indexBuffer = 0;
	//}

		// Release the instance buffer.
	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];


	// Set vertex buffer stride and offset.
	//stride = sizeof(VertexType);
	//offset = 0;

	// Set the buffer strides.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;
	//bufferPointers[2] = m_instanceCatBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the index buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, deviceContext, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}


	return;
}

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}