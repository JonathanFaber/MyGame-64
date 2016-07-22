#pragma once

#include "includes.h"
#include "Importer.hpp"      // C++ importer interface
#include "scene.h"           // Output data structure
#include "postprocess.h"     // Post processing flags
#include "vector3.h"


class Mesh {
public:
	std::vector <ModelVertex> vertices;
	std::vector <DWORD> indices;

	ID3D11Buffer* vertBuff;
	ID3D11Buffer* indexBuff;
	XMMATRIX meshWorld;
	ID3D11ShaderResourceView* texture_diff;
	ID3D11ShaderResourceView* texture_norm;
	ID3D11ShaderResourceView* texture_spec;

	UINT numVertices;
	UINT numIndices;

	void create() {
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
		vertexBufferDesc.ByteWidth = sizeof(ModelVertex)*numVertices;
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
		Translation = XMMatrixTranslation(-camPos.x, 5000000.0 - camPos.y, -camPos.z);

		meshWorld = Scale * Translation;
	}

	void draw() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(MODEL_VS, 0, 0);
		d3d11DevCon->PSSetShader(MODEL_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(modelLayout);

		UINT stride = sizeof(ModelVertex);
		UINT offset = 0;

		//Set the grounds index buffer
		d3d11DevCon->IASetIndexBuffer(indexBuff, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuff, &stride, &offset);

		//Set the WVP matrix and send it to the constant buffer in effect file
		WVP = meshWorld * camView * camProjection[0];
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		cbPerObj.World = XMMatrixTranspose(meshWorld);
		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		d3d11DevCon->PSSetShaderResources(0, 1, &texture_diff);
		d3d11DevCon->PSSetShaderResources(1, 1, &texture_norm);
		d3d11DevCon->PSSetShaderResources(2, 1, &texture_spec);
		d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

		d3d11DevCon->RSSetState(RSCullNone);
		d3d11DevCon->DrawIndexed(numIndices, 0, 0);
	}

	void cleanUp() {
		vertBuff->Release();
		indexBuff->Release();
		texture_diff->Release();
		texture_norm->Release();
		//texture_spec->Release();
	}
};

class Model {
	std::wstring filePath;
	std::vector <Mesh> mesh;
	UINT numMeshes;

	void setModelData(const aiScene *scene) {
		numMeshes = scene->mNumMeshes;

		for (UINT i = 0; i < numMeshes; i++) {
			Mesh tempMesh;
			tempMesh.numVertices = scene->mMeshes[i]->mNumVertices;
			tempMesh.numIndices = scene->mMeshes[i]->mNumFaces * 3;

			for (UINT j = 0; j < tempMesh.numVertices; j++) {
				tempMesh.vertices.push_back(ModelVertex(scene->mMeshes[i]->mVertices[j].x,
					scene->mMeshes[i]->mVertices[j].y,
					scene->mMeshes[i]->mVertices[j].z,
					scene->mMeshes[i]->mTextureCoords[0][j].x,
					scene->mMeshes[i]->mTextureCoords[0][j].y,
					scene->mMeshes[i]->mNormals[j].x,
					scene->mMeshes[i]->mNormals[j].y,
					scene->mMeshes[i]->mNormals[j].z,
					scene->mMeshes[i]->mTangents[j].x,
					scene->mMeshes[i]->mTangents[j].y,
					scene->mMeshes[i]->mTangents[j].z,
					scene->mMeshes[i]->mBitangents[j].x,
					scene->mMeshes[i]->mBitangents[j].y,
					scene->mMeshes[i]->mBitangents[j].z));
			}

			for (UINT j = 0; j < scene->mMeshes[i]->mNumFaces; j++) {
				tempMesh.indices.push_back(DWORD(scene->mMeshes[i]->mFaces[j].mIndices[0]));
				tempMesh.indices.push_back(DWORD(scene->mMeshes[i]->mFaces[j].mIndices[1]));
				tempMesh.indices.push_back(DWORD(scene->mMeshes[i]->mFaces[j].mIndices[2]));
			}

			bool texFound = false;
			UINT texCount = scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE)
				+ scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTextureCount(aiTextureType_NORMALS);

			aiString texFilePath;

			for (UINT j = 0; j < texCount; j++) {
				if (scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, j, &texFilePath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS && !texFound) {
					std::wstring fileName = filePath + boost::lexical_cast<std::wstring>(texFilePath.data);
					hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, fileName.c_str(),
						NULL, NULL, &tempMesh.texture_diff, NULL);
					texFound = true;
				}
			}
			if (!texFound) {
				hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/notexture_diff.png",
					NULL, NULL, &tempMesh.texture_diff, NULL);
			}

			texFound = false;

			for (UINT j = 0; j < texCount; j++) {
				for (UINT k = 0; k < scene->mNumMaterials; k++) {
					if (scene->mMaterials[k]->GetTexture(aiTextureType_NORMALS, j, &texFilePath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS && !texFound) {
						std::wstring fileName = filePath + boost::lexical_cast<std::wstring>(texFilePath.data);
						hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, fileName.c_str(),
							NULL, NULL, &tempMesh.texture_norm, NULL);
						texFound = true;
					}
				}
			}
			if (!texFound) {
				for (UINT j = 0; j < texCount; j++) {
					for (UINT k = 0; k < scene->mNumMaterials; k++) {
						if (scene->mMaterials[k]->GetTexture(aiTextureType_HEIGHT, j, &texFilePath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS && !texFound) {
							std::wstring fileName = filePath + boost::lexical_cast<std::wstring>(texFilePath.data);
							hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, fileName.c_str(),
								NULL, NULL, &tempMesh.texture_norm, NULL);
							texFound = true;
						}
					}
				}
			}
			if (!texFound) {
				hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/notexture_norm.png",
					NULL, NULL, &tempMesh.texture_norm, NULL);
			}

			texFound = false;

			for (UINT j = 0; j < texCount; j++) {
				if (scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTexture(aiTextureType_SPECULAR, j, &texFilePath, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS && !texFound) {
					std::wstring fileName = filePath + boost::lexical_cast<std::wstring>(texFilePath.data);
					hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, fileName.c_str(),
						NULL, NULL, &tempMesh.texture_spec, NULL);
					texFound = true;
				}
			}
			if (!texFound) {
				hr = D3DX11CreateShaderResourceViewFromFile(d3d11Device, L"textures/notexture_spec.png",
					NULL, NULL, &tempMesh.texture_norm, NULL);
			}

			mesh.push_back(tempMesh);
		}
	}

	bool importModel(const std::string& filePath1, const std::string& fileName) {
		// Create an instance of the Importer class
		Assimp::Importer importer;
		// And have it read the given file with some example postprocessing
		// Usually - if speed is not the most important aspect for you - you'll 
		// propably to request more postprocessing than we do in this example.
		const aiScene *scene = importer.ReadFile(filePath1 + fileName,
			aiProcess_CalcTangentSpace |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenSmoothNormals |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);

		// If the import failed, report it
		if (!scene)
		{
			return false;
		}

		// Now we can access the file's contents. 
		setModelData(scene);
		// We're done. Everything will be cleaned up by the importer destructor
		return true;
	}

public:
	void load(std::string filePath1, std::string fileName) {
		filePath = std::wstring(filePath1.begin(), filePath1.end()); // only works for ascii
		importModel(filePath1, fileName);

		for (UINT i = 0; i < numMeshes; i++)
			mesh[i].create();
	}

	void update() {
		for (UINT i = 0; i < numMeshes; i++)
			mesh[i].update();
	}

	void draw() {
		for (UINT i = 0; i < numMeshes; i++)
			mesh[i].draw();
	}

	void cleanUp() {
		for (UINT i = 0; i < numMeshes; i++)
			mesh[i].cleanUp();
	}
}testModel;