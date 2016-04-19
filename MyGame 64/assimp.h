#include "includes.h"

#ifndef __ASSIMP_H_INCLUDED__
#define __ASSIMP_H_INCLUDED__ 

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags
#include "assimp/vector3.h"

/*
bool importModel(const aiScene* scene, const std::string& pFile)
{
	// Create an instance of the Importer class
	Assimp::Importer importer;

	//const aiScene* scene;

	// And have it read the given file with some example postprocessing
	// Usually - if speed is not the most important aspect for you - you'll 
	// propably to request more postprocessing than we do in this example.
	scene = importer.ReadFile(pFile,
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	if (!scene)
	{
		printf("%s\n", importer.GetErrorString());
		return false;
	}

	// We're done. Everything will be cleaned up by the importer destructor
	return true;
}
*/

class mesh {
	std::vector <Vertex> vertices;
	std::vector <DWORD> indices;

	unsigned int numVertices;
	unsigned int numIndices;

	ID3D11Buffer* vertBuff;
	ID3D11Buffer* indexBuff;
	XMMATRIX meshWorld;
public:
	aiMesh meshData;

	mesh(){}

	void getData() {
		numVertices = meshData.mNumVertices;

		for (unsigned int i = 0; i < numVertices; i++) {
			vertices.push_back(Vertex(meshData.mVertices[i].x, meshData.mVertices[i].y, meshData.mVertices[i].z,
				meshData.mTextureCoords[0][i].x, meshData.mTextureCoords[0][i].y,
				meshData.mNormals[i].x, meshData.mNormals[i].y, meshData.mNormals[i].z));
		}

		numIndices = meshData.mNumFaces * 3;

		for (unsigned int i = 0; i < meshData.mNumFaces; i++) {
			aiFace face = meshData.mFaces[i];
			indices.push_back(DWORD(face.mIndices[0]));
			indices.push_back(DWORD(face.mIndices[1]));
			indices.push_back(DWORD(face.mIndices[2]));
		}

	}

	void init() {
		//Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD)*numIndices;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = &indices[0];
		d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuff);

		//Create Vertex Buffer
		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex)*numVertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		vertexBufferData.pSysMem = &vertices[0];
		hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertBuff);
	}

	void update() {
		meshWorld = XMMatrixIdentity();

		//Define cube1's world space matrix
		Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		Translation = XMMatrixTranslation(-camPos.x, -camPos.y, -camPos.z);

		meshWorld = Scale * Translation;
	}

	void draw() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(VS, 0, 0);
		d3d11DevCon->PSSetShader(PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(vertLayout);

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		//Set the grounds index buffer
		d3d11DevCon->IASetIndexBuffer(indexBuff, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);

		//Set the WVP matrix and send it to the constant buffer in effect file
		WVP = meshWorld * camView * camProjection;
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		cbPerObj.World = XMMatrixTranspose(meshWorld);
		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		d3d11DevCon->PSSetShaderResources(0, 1, &texture[6]);
		//d3d11DevCon->PSSetShaderResources(1, 1, &texture[1]);
		//d3d11DevCon->PSSetShaderResources(2, 1, &texture[2]);
		//d3d11DevCon->PSSetShaderResources(3, 1, &texture[3]);
		//d3d11DevCon->PSSetShaderResources(4, 1, &texture[4]);
		d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

		d3d11DevCon->RSSetState(RSCullNone);
		d3d11DevCon->DrawIndexed(numIndices, 0, 0);
	}

	void cleanUp() {
		vertBuff->Release();
		indexBuff->Release();
	}

};


class model {
public:
	std::vector <mesh> meshes;

	const aiScene* scene0;
	aiScene scene;


	void init(const std::string& pFile) {
		//importModel(scene, pFile);

		// Create an instance of the Importer class
		Assimp::Importer importer;

		scene0 = importer.ReadFile(pFile,
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			//aiProcess_MakeLeftHanded |
			aiProcess_GenSmoothNormals);

		scene = *scene0;

		meshes.resize(scene.mNumMeshes);

		for (unsigned int i = 0; i < scene.mNumMeshes; i++) {
			meshes[i].meshData = *scene.mMeshes[i];
			meshes[i].getData();
			meshes[i].init();
		}
	}

	void update() {
		for (unsigned int i = 0; i < scene.mNumMeshes; i++)
			meshes[i].update();
	}

	void draw() {
		for (unsigned int i = 0; i < scene.mNumMeshes; i++)
			meshes[i].draw();
	}

	void cleanUp() {
		for (unsigned int i = 0; i < scene.mNumMeshes; i++)
			meshes[i].cleanUp();
	}
	
	
}sprucetree;





#endif