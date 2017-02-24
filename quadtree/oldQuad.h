#pragma once

#include "includes.h"
#include "shaders.h"
#include "terrain.h"

struct Quad {
	double3 pos;
	double3 posSquare;
	double3 posAway;
	double distance;
	double length;
	bool subdivide;
	bool combine;
	bool draw;

	Quad() {}
	Quad(double3 pos1, double3 posSquare1, double length1) : pos(pos1), posSquare(posSquare1), length(length1) {}

	void update() {
		posAway = pos - camPos;
		distance = vLength(posAway);

		if (distance < length*4.0 && length > minLength) {
			subdivide = true;
			combine = false;
		}
		else if (distance > length*12.0 && length < maxLength) {
			combine = true;
			subdivide = false;
		}
		else {
			subdivide = false;
			combine = false;
		}
	}

private:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertBuffer;
	XMMATRIX groundWorld;
	XMFLOAT3 translation;

	PlanetVertex verticesInitial[(chunkLength + 1)*(chunkLength + 1)];
	DWORD indices[chunkLength*chunkLength * 6];

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA indexBufferData;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA initialVertexBufferData;
	D3D11_MAPPED_SUBRESOURCE updatedVertexBufferData;

public:
	PlanetVertex verticesFinal[(chunkLength + 1)*(chunkLength + 1)];
	XMFLOAT3 positions[chunkLength + 3][chunkLength + 3];
	double3 firstCamPos;
	bool terrainCalculated;
	bool firstUpdate;

	void create(char side) {
		if (side == 'y') {
			counter = 0;
			for (int z = 0; z < chunkLength + 1; z++) {
				for (int x = 0; x < chunkLength + 1; x++) {
					verticesInitial[counter] = PlanetVertex(float(x - chunkLength / 2) / 16.0f, 0.0f, float(z - chunkLength / 2) / 16.0f, float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
					counter++;
				}
			}
		}
		else if (side == 'x') {
			counter = 0;
			for (int z = 0; z < chunkLength + 1; z++) {
				for (int x = 0; x < chunkLength + 1; x++) {
					verticesInitial[counter] = PlanetVertex(0.0f, float(z - chunkLength / 2) / 16.0f, float(x - chunkLength / 2) / 16.0f, float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
					counter++;
				}
			}
		}
		else {
			counter = 0;
			for (int z = 0; z < chunkLength + 1; z++) {
				for (int x = 0; x < chunkLength + 1; x++) {
					verticesInitial[counter] = PlanetVertex(float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 0.0f, float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
					counter++;
				}
			}
		}

		counter = 0;
		// in this order for gs shader
		for (int x = 0; x < chunkLength; x++) {
			for (int z = 0; z < chunkLength; z++) {
				indices[counter] = x + z * (chunkLength + 1); // 0
				counter++;
				indices[counter] = x + 1 + z * (chunkLength + 1); // 1
				counter++;
				indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1); // 2
				counter++;
				indices[counter] = x + z * (chunkLength + 1); // 0
				counter++;
				indices[counter] = x + (chunkLength + 1) + z * (chunkLength + 1); // 3
				counter++;
				indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1); // 2
				counter++;
			}
		}

		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.ByteWidth = sizeof(indices);

		indexBufferData.pSysMem = indices;
		d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);

		ZeroMemory(&initialVertexBufferData, sizeof(initialVertexBufferData));
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
		ZeroMemory(&updatedVertexBufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.ByteWidth = sizeof(verticesInitial);

		initialVertexBufferData.pSysMem = verticesInitial;
		hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &initialVertexBufferData, &vertBuffer);
	}

	void transform(double3 camPos) {
		groundWorld = XMMatrixIdentity();
		Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		Translation = XMMatrixTranslation(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
		translation = XMFLOAT3(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
		//Translation = XMMatrixTranslation(0.0f, -5.0f, 0.0f);
		groundWorld = Scale * Translation;
	}

	void updateChunkData(char side) {
		counter = 0;
		for (int z = 0; z < (chunkLength + 3); z++) {
			for (int x = 0; x < (chunkLength + 3); x++) {
				double3 temp = posSquare;

				if (side == 'y')
					temp = double3((x - 1 - chunkLength / 2) / 16.0 * length, 0.0, (z - 1 - chunkLength / 2) / 16.0 * length);
				else if (side == 'x')
					temp = double3(0.0, (z - 1 - chunkLength / 2) / 16.0 * length, (x - 1 - chunkLength / 2) / 16.0 * length);
				else
					temp = double3((x - 1 - chunkLength / 2) / 16.0 * length, (z - 1 - chunkLength / 2) / 16.0 * length, 0.0);

				temp = temp + posSquare;



				temp.x /= maxLength;
				temp.y /= maxLength;
				temp.z /= maxLength;

				temp = spherize(temp);


				double3 tempNormal = temp;

				temp = temp * maxLength;

				terrainPoint.generateTerrainPoint(tempNormal);

				temp = temp + terrainPoint.terrain;
				temp = temp - firstCamPos;

				positions[x][z] = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));

				if (x != 0 && z != 0 && x != chunkLength + 2 && z != chunkLength + 2) {
					verticesFinal[counter].pos = positions[x][z];
					if (side == 'y')
						verticesFinal[counter].texCoord = XMFLOAT2(float(double(verticesInitial[counter].pos.x) * length), float(double(verticesInitial[counter].pos.z) * length));
					else if (side == 'x')
						verticesFinal[counter].texCoord = XMFLOAT2(float(double(verticesInitial[counter].pos.z) * length), float(double(verticesInitial[counter].pos.y) * length));
					else
						verticesFinal[counter].texCoord = XMFLOAT2(float(double(verticesInitial[counter].pos.x) * length), float(double(verticesInitial[counter].pos.y) * length));

					verticesFinal[counter].height = terrainPoint.height_f;
					verticesFinal[counter].landTypeHeight = terrainPoint.landTypeHeight_f;
					counter++;
				}
			}
		}

		XMFLOAT3 V;
		XMFLOAT3 W;
		XMFLOAT3 f_normals[2][chunkLength + 2][chunkLength + 2];
		XMFLOAT3 normals[chunkLength + 1][chunkLength + 1];

		counter = 0;
		for (int z = 0; z < chunkLength + 2; z++) {
			for (int x = 0; x < chunkLength + 2; x++) {
				for (int i = 0; i < 2; i++) {
					if (i == 0) {
						V.x = positions[x + 1][z + 1].x - positions[x][z].x;
						V.y = positions[x + 1][z + 1].y - positions[x][z].y;
						V.z = positions[x + 1][z + 1].z - positions[x][z].z;
						W.x = positions[x + 1][z].x - positions[x][z].x;
						W.y = positions[x + 1][z].y - positions[x][z].y;
						W.z = positions[x + 1][z].z - positions[x][z].z;
					}
					if (i == 1) {
						V.x = positions[x + 1][z + 1].x - positions[x][z].x;
						V.y = positions[x + 1][z + 1].y - positions[x][z].y;
						V.z = positions[x + 1][z + 1].z - positions[x][z].z;
						W.x = positions[x][z + 1].x - positions[x][z].x;
						W.y = positions[x][z + 1].y - positions[x][z].y;
						W.z = positions[x][z + 1].z - positions[x][z].z;
					}
					f_normals[i][x][z] = crossProduct(V, W);
					f_normals[i][x][z] = normalize(f_normals[i][x][z]);

					if (dotProduct(f_normals[i][x][z], XMFLOAT3(pos.x, pos.y, pos.z)) < 0.0f) {
						f_normals[i][x][z].x *= -1;
						f_normals[i][x][z].y *= -1;
						f_normals[i][x][z].z *= -1;
					}

				}
			}
		}

		counter = 0;
		for (int z = 1; z < chunkLength + 2; z++) {
			for (int x = 1; x < chunkLength + 2; x++) {
				normals[x - 1][z - 1].x = ((f_normals[0][x - 1][z - 1].x + f_normals[1][x - 1][z - 1].x) / 2 + f_normals[0][x - 1][z].x + (f_normals[0][x][z].x + f_normals[1][x][z].x) / 2 + f_normals[1][x][z - 1].x) / 4;
				normals[x - 1][z - 1].y = ((f_normals[0][x - 1][z - 1].y + f_normals[1][x - 1][z - 1].y) / 2 + f_normals[0][x - 1][z].y + (f_normals[0][x][z].y + f_normals[1][x][z].y) / 2 + f_normals[1][x][z - 1].y) / 4;
				normals[x - 1][z - 1].z = ((f_normals[0][x - 1][z - 1].z + f_normals[1][x - 1][z - 1].z) / 2 + f_normals[0][x - 1][z].z + (f_normals[0][x][z].z + f_normals[1][x][z].z) / 2 + f_normals[1][x][z - 1].z) / 4;
				normals[x - 1][z - 1] = normalize(normals[x - 1][z - 1]);

				verticesFinal[counter].normal = normals[x - 1][z - 1];
				counter++;
			}
		}

		d3d11DevCon->Map(vertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &updatedVertexBufferData);
		memcpy(updatedVertexBufferData.pData, verticesFinal, sizeof(verticesFinal));
		d3d11DevCon->Unmap(vertBuffer, 0);

		terrainCalculated = true;
	}

	void drawTerrain() {
		UINT stride = sizeof(PlanetVertex);
		UINT offset = 0;

		d3d11DevCon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);

		//Set the WVP matrix and send it to the constant buffer in effect file
		//WVP = groundWorld * camView * camProjection;
		if (distance < 900.0f)
			WVP = groundWorld * camView * camProjection[0];
		else
			WVP = groundWorld * camView * camProjection[1];

		//cbPerPlanetObj.WVP = XMMatrixTranspose(WVP);
		//cbPerPlanetObj.World = XMMatrixTranspose(groundWorld);
		cbPerPlanetObj.Translation = translation;
		cbPerPlanetObj.View = XMMatrixTranspose(camView);

		if (distance < 900.0f)
			cbPerPlanetObj.Proj = XMMatrixTranspose(camProjection[0]);
		else
			cbPerPlanetObj.Proj = XMMatrixTranspose(camProjection[1]);

		d3d11DevCon->UpdateSubresource(cbPerPlanetBuffer, 0, NULL, &cbPerPlanetObj, 0, 0);
		d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerPlanetBuffer);
		d3d11DevCon->GSSetConstantBuffers(0, 1, &cbPerPlanetBuffer);
		d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
		d3d11DevCon->PSSetShaderResources(1, 1, &texture[2]);
		d3d11DevCon->PSSetShaderResources(2, 1, &texture[6]);
		d3d11DevCon->PSSetShaderResources(3, 1, &texture[5]);
		d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

		d3d11DevCon->RSSetState(RSCullNone);
		//if (posSquare.y == maxLength || posSquare.x == maxLength || posSquare.z == -maxLength)
		//	d3d11DevCon->RSSetState(CCWcullMode);
		//else
		//	d3d11DevCon->RSSetState(CWcullMode);
		//d3d11DevCon->RSSetState(Wireframe);
		d3d11DevCon->DrawIndexed(chunkLength*chunkLength * 6, 0, 0);
	}

	void cleanUp() {
		vertBuffer->Release();
		indexBuffer->Release();
	}
};
