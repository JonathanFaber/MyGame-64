#include "includes.h"
#include "planet.h"
#include "shaders.h"

#ifndef __GRASS_H_INCLUDED__
#define __GRASS_H_INCLUDED__ 

class Grass {

	ID3D11Buffer* grassIndexBuffer;
	ID3D11Buffer* grassVertBuffer;

	InstanceData grassPos[20000];
	int numGrassPatches = 20000;

	XMMATRIX grassWorld;

	D3D11_BUFFER_DESC grassIndexBufferDesc;
	D3D11_SUBRESOURCE_DATA grassIndexBufferData;
	D3D11_BUFFER_DESC grassVertexBufferDesc;
	D3D11_SUBRESOURCE_DATA grassVertexBufferData;

	D3D11_BUFFER_DESC instancebd;
	D3D11_MAPPED_SUBRESOURCE InitInstanceData;
	D3D11_SUBRESOURCE_DATA InitialInstanceData;

public:
	void create() {
		/*
		Vertex grassVertices[24];

		grassVertices[0].pos.x = 0.0f;
		grassVertices[0].pos.y = 0.0f;
		grassVertices[0].pos.z = 0.5f;
		grassVertices[0].texCoord.x = 0.0f;
		grassVertices[0].texCoord.y = 0.0f;

		grassVertices[1].pos.x = 1.0f;
		grassVertices[1].pos.y = 0.0f;
		grassVertices[1].pos.z = 0.5f;
		grassVertices[1].texCoord.x = 1.0f;
		grassVertices[1].texCoord.y = 0.0f;

		grassVertices[2].pos.x = 0.5f;
		grassVertices[2].pos.y = 0.0f;
		grassVertices[2].pos.z = 0.0f;
		grassVertices[2].texCoord.x = 0.0f;
		grassVertices[2].texCoord.y = 0.0f;

		grassVertices[3].pos.x = 0.5f;
		grassVertices[3].pos.y = 0.0f;
		grassVertices[3].pos.z = 1.0f;
		grassVertices[3].texCoord.x = 1.0f;
		grassVertices[3].texCoord.y = 0.0f;

		grassVertices[4].pos.x = 0.0f;
		grassVertices[4].pos.y = 1.0f;
		grassVertices[4].pos.z = 0.0f;
		grassVertices[4].texCoord.x = 0.0f;
		grassVertices[4].texCoord.y = 1.0f;

		grassVertices[5].pos.x = 1.0f;
		grassVertices[5].pos.y = 1.0f;
		grassVertices[5].pos.z = 0.0f;
		grassVertices[5].texCoord.x = 1.0f;
		grassVertices[5].texCoord.y = 1.0f;

		grassVertices[6].pos.x = 0.0f;
		grassVertices[6].pos.y = 1.0f;
		grassVertices[6].pos.z = 1.0f;
		grassVertices[6].texCoord.x = 0.0f;
		grassVertices[6].texCoord.y = 1.0f;

		grassVertices[7].pos.x = 1.0f;
		grassVertices[7].pos.y = 1.0f;
		grassVertices[7].pos.z = 1.0f;
		grassVertices[7].texCoord.x = 1.0f;
		grassVertices[7].texCoord.y = 1.0f;

		grassVertices[8].pos.x = 0.0f;
		grassVertices[8].pos.y = 1.0f;
		grassVertices[8].pos.z = 0.0f;
		grassVertices[8].texCoord.x = 0.0f;
		grassVertices[8].texCoord.y = 1.0f;

		grassVertices[9].pos.x = 1.0f;
		grassVertices[9].pos.y = 1.0f;
		grassVertices[9].pos.z = 0.0f;
		grassVertices[9].texCoord.x = 0.0f;
		grassVertices[9].texCoord.y = 1.0f;

		grassVertices[10].pos.x = 0.0f;
		grassVertices[10].pos.y = 1.0f;
		grassVertices[10].pos.z = 1.0f;
		grassVertices[10].texCoord.x = 1.0f;
		grassVertices[10].texCoord.y = 1.0f;

		grassVertices[11].pos.x = 1.0f;
		grassVertices[11].pos.y = 1.0f;
		grassVertices[11].pos.z = 1.0f;
		grassVertices[11].texCoord.x = 1.0f;
		grassVertices[11].texCoord.y = 1.0f;

		grassVertices[0+12].pos.x = 0.0f;
		grassVertices[0+12].pos.y = 0.0f;
		grassVertices[0+12].pos.z = 0.0f;
		grassVertices[0+12].texCoord.x = 0.0f;
		grassVertices[0+12].texCoord.y = 0.0f;

		grassVertices[1+12].pos.x = 1.0f;
		grassVertices[1+12].pos.y = 0.0f;
		grassVertices[1+12].pos.z = 0.0f;
		grassVertices[1+12].texCoord.x = 1.0f;
		grassVertices[1+12].texCoord.y = 0.0f;

		grassVertices[2+12].pos.x = 0.0f;
		grassVertices[2+12].pos.y = 0.0f;
		grassVertices[2+12].pos.z = 0.0f;
		grassVertices[2+12].texCoord.x = 0.0f;
		grassVertices[2+12].texCoord.y = 0.0f;

		grassVertices[3+12].pos.x = 0.0f;
		grassVertices[3+12].pos.y = 0.0f;
		grassVertices[3+12].pos.z = 1.0f;
		grassVertices[3+12].texCoord.x = 1.0f;
		grassVertices[3+12].texCoord.y = 0.0f;

		grassVertices[4+12].pos.x = 0.0f;
		grassVertices[4+12].pos.y = 1.0f;
		grassVertices[4+12].pos.z = -0.5f;
		grassVertices[4+12].texCoord.x = 0.0f;
		grassVertices[4+12].texCoord.y = 1.0f;

		grassVertices[5+12].pos.x = 1.0f;
		grassVertices[5+12].pos.y = 1.0f;
		grassVertices[5+12].pos.z = -0.5f;
		grassVertices[5+12].texCoord.x = 1.0f;
		grassVertices[5+12].texCoord.y = 1.0f;

		grassVertices[6+12].pos.x = 0.0f;
		grassVertices[6+12].pos.y = 1.0f;
		grassVertices[6+12].pos.z = 0.5f;
		grassVertices[6+12].texCoord.x = 0.0f;
		grassVertices[6+12].texCoord.y = 1.0f;

		grassVertices[7+12].pos.x = 1.0f;
		grassVertices[7+12].pos.y = 1.0f;
		grassVertices[7+12].pos.z = 0.5f;
		grassVertices[7+12].texCoord.x = 1.0f;
		grassVertices[7+12].texCoord.y = 1.0f;

		grassVertices[8+12].pos.x = -0.5f;
		grassVertices[8+12].pos.y = 1.0f;
		grassVertices[8+12].pos.z = 0.0f;
		grassVertices[8+12].texCoord.x = 0.0f;
		grassVertices[8+12].texCoord.y = 1.0f;

		grassVertices[9+12].pos.x = 0.5f;
		grassVertices[9+12].pos.y = 1.0f;
		grassVertices[9+12].pos.z = 0.0f;
		grassVertices[9+12].texCoord.x = 0.0f;
		grassVertices[9+12].texCoord.y = 1.0f;

		grassVertices[10+12].pos.x = -0.5f;
		grassVertices[10+12].pos.y = 1.0f;
		grassVertices[10+12].pos.z = 1.0f;
		grassVertices[10+12].texCoord.x = 1.0f;
		grassVertices[10+12].texCoord.y = 1.0f;

		grassVertices[11+12].pos.x = 0.5f;
		grassVertices[11+12].pos.y = 1.0f;
		grassVertices[11+12].pos.z = 1.0f;
		grassVertices[11+12].texCoord.x = 1.0f;
		grassVertices[11+12].texCoord.y = 1.0f;

		for (int i = 0; i < 24; i++){
		grassVertices[i].normal.x = 0.0f;
		grassVertices[i].normal.y = 1.0f;
		grassVertices[i].normal.z = 0.0f;
		grassVertices[i].texID = 4;
		}

		DWORD grassIndices[48] = {
		0, 1, 5,
		0, 5, 4,
		0, 1, 7,
		0, 7, 6,
		2, 3, 10,
		2, 10, 8,
		2, 3, 11,
		2, 11, 9,
		//
		0+12, 1+12, 5+12,
		0+12, 5+12, 4+12,
		0+12, 1+12, 7+12,
		0+12, 7+12, 6+12,
		2+12, 3+12, 10+12,
		2+12, 10+12, 8+12,
		2+12, 3+12, 11+12,
		2+12, 11+12, 9+12,
		};


		//Positions per instance
		counter = 0;
		for (int x = 0; x < 200; x++){
		for (int z = 0; z < 200; z++){
		grassPos[counter].instancePos = XMFLOAT2(float(-x), float(-z));
		grassPos[counter].instanceHeights = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		grassPos[counter].instanceRot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		counter++;
		}
		}
		*/

		Vertex grassVertices[16];

		counter = 0;
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.05f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//1
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.05f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//2
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.05f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//3
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.05f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;
		//
		counter++;//4
		grassVertices[counter].pos.x = 0.05f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//5
		grassVertices[counter].pos.x = 0.05f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//6
		grassVertices[counter].pos.x = 0.05f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//7
		grassVertices[counter].pos.x = 0.05f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;
		//
		counter++;//8
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//9
		grassVertices[counter].pos.x = 0.1f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//10
		grassVertices[counter].pos.x = 0.1f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.1f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//11
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.1f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;
		//
		counter++;//12
		grassVertices[counter].pos.x = -0.1f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = -0.01f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0;

		counter++;//13
		grassVertices[counter].pos.x = 0.2f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = -0.1f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//14
		grassVertices[counter].pos.x = 0.2f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.2f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//15
		grassVertices[counter].pos.x = -0.1f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.2f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		for (int i = 0; i < 16; i++) {
			grassVertices[i].normal.x = 0.0f;
			grassVertices[i].normal.y = 1.0f;
			grassVertices[i].normal.z = 0.0f;
		}

		DWORD grassIndices[12] = {
			0, 1, 3,
			0, 3, 2,
			4, 5, 7,
			4, 7, 6,
			/*
			8, 9, 13,
			8, 13, 12,
			9, 10, 14,
			9, 14, 13,
			10, 11, 15,
			10, 15, 14,
			11, 0, 12,
			11, 12, 15,
			*/
		};

		//Positions per instance
		counter = 0;
		for (int x = 0; x < 100; x++) {
			for (int z = 0; z < 100; z++) {
				grassPos[counter].instancePos = XMFLOAT2(float(-x), float(-z));
				grassPos[counter].instanceHeights = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
				grassPos[counter].instanceRot = XMFLOAT3(0.0f, 0.0f, 0.0f);
				counter++;
			}
		}

		/*
		Vertex grassVertices[32];

		counter = 0;
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//1
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//2
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = 0.55f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//3
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = 0.55f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//4
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = 0.65f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//5
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = 0.65f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//6
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.8f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//7
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.8f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;
		//
		counter++;//8
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//9
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//10
		grassVertices[counter].pos.x = -0.04f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = 0.04f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//11
		grassVertices[counter].pos.x = 0.96f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = 1.04f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//12
		grassVertices[counter].pos.x = -0.11f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = 0.11f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//13
		grassVertices[counter].pos.x = 0.89f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = 1.11f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//14
		grassVertices[counter].pos.x = -0.22f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.22f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//15
		grassVertices[counter].pos.x = 0.78f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 1.22f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;
		//
		counter++;//16
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//17
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//18
		grassVertices[counter].pos.x = 0.45f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//19
		grassVertices[counter].pos.x = 0.45f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//20
		grassVertices[counter].pos.x = 0.35f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//21
		grassVertices[counter].pos.x = 0.35f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//22
		grassVertices[counter].pos.x = 0.2f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//23
		grassVertices[counter].pos.x = 0.2f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;
		//
		counter++;//24
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//25
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//26
		grassVertices[counter].pos.x = 0.96f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = -0.04f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//27
		grassVertices[counter].pos.x = -0.04f;
		grassVertices[counter].pos.y = 0.33f;
		grassVertices[counter].pos.z = 0.96f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.33f;

		counter++;//28
		grassVertices[counter].pos.x = 0.89f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = -0.11f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//29
		grassVertices[counter].pos.x = -0.11f;
		grassVertices[counter].pos.y = 0.67f;
		grassVertices[counter].pos.z = 0.89f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.67f;

		counter++;//30
		grassVertices[counter].pos.x = 0.78f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = -0.22f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//31
		grassVertices[counter].pos.x = -0.22f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.78f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		for (int i = 0; i < 32; i++){
		grassVertices[i].normal.x = 0.0f;
		grassVertices[i].normal.y = 1.0f;
		grassVertices[i].normal.z = 0.0f;
		grassVertices[i].texID = 4;
		}

		DWORD grassIndices[72];

		counter = 0;
		for (int j = 0; j < 4; j++){
		for (int i = 0; i < 3; i++){
		grassIndices[counter] = 0+i*2+j*8;
		counter++;
		grassIndices[counter] = 1+i*2+j*8;
		counter++;
		grassIndices[counter] = 3+i*2+j*8;
		counter++;
		grassIndices[counter] = 0+i*2+j*8;
		counter++;
		grassIndices[counter] = 3+i*2+j*8;
		counter++;
		grassIndices[counter] = 2+i*2+j*8;
		counter++;
		}
		}
		*/

		/*
		Vertex grassVertices[14];

		counter = 0;
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//1
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//2
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 0.0f;

		counter++;//3
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 0.0f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 0.0f;
		//
		counter++;//4
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//5
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.5f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//6
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//7
		grassVertices[counter].pos.x = 0.5f;
		grassVertices[counter].pos.y = 1.0f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;
		//
		counter++;//8
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.75f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//9
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.75f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//10
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.75f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//11
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.75f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 1.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//12
		grassVertices[counter].pos.x = 1.0f;
		grassVertices[counter].pos.y = 0.75f;
		grassVertices[counter].pos.z = 0.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;

		counter++;//13
		grassVertices[counter].pos.x = 0.0f;
		grassVertices[counter].pos.y = 0.75f;
		grassVertices[counter].pos.z = 1.0f;
		grassVertices[counter].texCoord.x = 0.0f;
		grassVertices[counter].texCoord.y = 1.0f;


		for (int i = 0; i < 14; i++){
		grassVertices[i].normal.x = 0.0f;
		grassVertices[i].normal.y = 1.0f;
		grassVertices[i].normal.z = 0.0f;
		grassVertices[i].texID = 0;
		}

		DWORD grassIndices[36] = {
		0, 1, 5,
		0, 5, 4,
		2, 3, 7,
		2, 7, 6,

		0, 1, 9,
		0, 9, 8,
		0, 1, 10,
		0, 10, 13,

		2, 3, 11,
		2, 11, 8,
		2, 3, 10,
		2, 10, 12,
		};
		*/

		ZeroMemory(&grassIndexBufferDesc, sizeof(grassIndexBufferDesc));

		grassIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		grassIndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		grassIndexBufferDesc.CPUAccessFlags = 0;
		grassIndexBufferDesc.MiscFlags = 0;
		grassIndexBufferDesc.ByteWidth = sizeof(grassIndices);

		grassIndexBufferData.pSysMem = grassIndices;
		d3d11Device->CreateBuffer(&grassIndexBufferDesc, &grassIndexBufferData, &grassIndexBuffer);

		ZeroMemory(&grassVertexBufferDesc, sizeof(grassVertexBufferDesc));

		grassVertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		grassVertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		grassVertexBufferDesc.CPUAccessFlags = 0;
		grassVertexBufferDesc.MiscFlags = 0;
		grassVertexBufferDesc.ByteWidth = sizeof(grassVertices);

		ZeroMemory(&grassVertexBufferData, sizeof(grassVertexBufferData));
		grassVertexBufferData.pSysMem = grassVertices;
		hr = d3d11Device->CreateBuffer(&grassVertexBufferDesc, &grassVertexBufferData, &grassVertBuffer);

		//Create the buffer to send to the cbuffer in effect file
		ZeroMemory(&InitInstanceData, sizeof(D3D11_MAPPED_SUBRESOURCE));
		ZeroMemory(&InitialInstanceData, sizeof(D3D11_SUBRESOURCE_DATA));
		ZeroMemory(&instancebd, sizeof(D3D11_BUFFER_DESC));

		instancebd.Usage = D3D11_USAGE_DYNAMIC;
		instancebd.ByteWidth = sizeof(grassPos);
		instancebd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		instancebd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		instancebd.MiscFlags = 0;
		InitialInstanceData.pSysMem = &grassPos;
		InitialInstanceData.SysMemPitch = 0;
		InitialInstanceData.SysMemSlicePitch = 0;
		hr = d3d11Device->CreateBuffer(&instancebd, &InitialInstanceData, &cbInstancePosBuffer);
	}

	XMFLOAT2 windDirection = XMFLOAT2(1.0f, 1.0f);
	float windSpeed = 0.0f;
	float windIntensity = 0.125f;

	void update(double3 camPos, double timeElaps) {
		windDirection.x = cos(timeElaps*windIntensity);
		windDirection.y = sin(timeElaps*windIntensity);
		windDirection = normalize(windDirection);

		counter = 0;

		for (int i = 0; i < 2048; i++) {
			if (planet.sideY1.quadData[i].draw == true && planet.sideY1.quadData[i].firstUpdate == false) {
				if (planet.sideY1.quadData[i].length == minLength) {
					for (int z = 0; z < chunkLength + 1; z++) {
						for (int x = 0; x < chunkLength + 1; x++) {
							if (planet.sideY1.quadData[i].verticesFinal[z * (chunkLength + 1) + x].landTypeHeight <= 0.5f) {
								double3 temp = planet.sideY1.quadData[i].position[x][z];
								temp.x += planet.sideY1.quadData[i].firstCamPos.x - camPos.x;
								temp.y += planet.sideY1.quadData[i].firstCamPos.y - camPos.y;
								temp.z += planet.sideY1.quadData[i].firstCamPos.z - camPos.z;

								if (vLength(temp) < 50.0) {
									grassPos[counter].instancePos = XMFLOAT2(float(temp.x), float(temp.z));
									grassPos[counter].instanceHeights.x = float(temp.y);
									grassPos[counter].instanceHeights.y = float(temp.y);
									grassPos[counter].instanceHeights.z = float(temp.y);
									grassPos[counter].instanceHeights.w = float(temp.y);
									grassPos[counter].instanceRot = XMFLOAT3(0.0f, 0.f, 0.0f);
									counter++;
								}
							}
						}
					}
				}
			}
		}
		
		numGrassPatches = counter;

		d3d11DevCon->Map(cbInstancePosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &InitInstanceData);
		memcpy(InitInstanceData.pData, grassPos, sizeof(InstanceData)*numGrassPatches);
		d3d11DevCon->Unmap(cbInstancePosBuffer, 0);

		//Reset grassWorld
		grassWorld = XMMatrixIdentity();

		//Define grassWorld's world space matrix
		Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		//Translation = XMMatrixTranslation(-camPos.x, -camPos.y, -camPos.z);
		Translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		//Set grassWorld's world space using the transformations
		grassWorld = Scale * Translation;
	}

	void draw() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(INSTANCE_VS, 0, 0);
		d3d11DevCon->PSSetShader(INSTANCE_PS, 0, 0);

		UINT strides[2] = { sizeof(Vertex), sizeof(InstanceData) };
		UINT offsets[2] = { 0, 0 };

		// Store the vertex and instance buffers into an array
		ID3D11Buffer* vertInstBuffers[2] = { grassVertBuffer, cbInstancePosBuffer };

		d3d11DevCon->IASetInputLayout(instanceLayout);

		//Set the cubes index buffer
		d3d11DevCon->IASetIndexBuffer(grassIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//Set the cubes vertex buffers
		d3d11DevCon->IASetVertexBuffers(0, 2, vertInstBuffers, strides, offsets);

		//Set the WVP matrix and send it to the constant buffer in effect file
		//WVP = grassWorld * camView * camProjection;
		WVP = grassWorld * camView * planetCamProjection;
		cbPerObj.WVP = XMMatrixTranspose(WVP);
		cbPerObj.World = XMMatrixTranspose(grassWorld);
		d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
		d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
		d3d11DevCon->PSSetShaderResources(0, 1, &texture[7]);
		d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

		d3d11DevCon->RSSetState(RSCullNone);
		d3d11DevCon->DrawIndexedInstanced(12, numGrassPatches, 0, 0, 0);
	}

	void cleanUp() {
		grassVertBuffer->Release();
		grassIndexBuffer->Release();
	}

}grass;

#endif
