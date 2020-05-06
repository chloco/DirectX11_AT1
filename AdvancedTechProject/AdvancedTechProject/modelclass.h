#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;
#include "textureclass.h"
#include <fstream>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass 
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct InstanceType
	{
		XMFLOAT3 position;
		XMFLOAT3 scale;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();


	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, char*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	void setPosition(float, float ,float);
	void setScale(float, float, float);

	void doSize();

	XMFLOAT3 getPosition();
	XMFLOAT3 getScale();

	int GetIndexCount();

	int GetVertexCount();
	int GetInstanceCount();
	int GetCatInstanceCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, char*);
	void ReleaseTexture();

	//loading and unlading the model data from text file
	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_instanceBuffer;
	ID3D11Buffer* m_instanceCatBuffer;
	XMMATRIX Scale();
	int m_vertexCount, m_indexCount;

	TextureClass* m_Texture;
	int m_instanceCount;
	int m_instanceCountCats;
	ModelType* m_model;

	float xPos;
	float yPos;
	float zPos;

	float xScale;
	float yScale;
	float zScale;

	XMMATRIX cat;
	XMMATRIX treadmill;

	XMMATRIX Rotation;
	XMMATRIX Scale;
	XMMATRIX Translation;
};

#endif