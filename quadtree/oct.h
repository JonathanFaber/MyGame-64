//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_OCTTREE_H
#define MYGAME_64_OCTTREE_H

#include "includes.h"
#include "terrain.h"
#include "transvoxel.h"

#define N_CHILDREN 8

class Oct {
public:
    Oct() {}
    Oct(double3 pos, double length);
    ~Oct();

    void update();
    void transform();
    void drawTerrain(bool drawClose);
	void addOcts(int *added, bool onlyVisible);
	void removeOcts(int len);

private:
	double3 pos;
	double3 posAway;
	double distance;
	double length;
	double3 firstCamPos = camPos;
	bool subdivide = false;
	bool combine = false;
	bool draw = true;
	Oct *child[N_CHILDREN];

    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertBuffer;
    XMMATRIX groundWorld;
    XMFLOAT3 translation;

    PlanetVertex vertices[(chunkLength)*(chunkLength)*(chunkLength)*12];
    DWORD indices[chunkLength*chunkLength*chunkLength * 6 * 3];
	int nVertices = 0;
	int nIndices = 0;

    D3D11_SUBRESOURCE_DATA indexBufferData;
    D3D11_SUBRESOURCE_DATA vertexBufferData;

	void setTerrainData();
	void makeChildren();
	TerrainPoint interpolateVertex(TerrainPoint a, TerrainPoint b, float threshold);
};

inline Oct::Oct(double3 pos, double length) {
    this->pos = pos;
    this->length = length;
	draw = true;
	subdivide = false;
	combine = false;

	for (int i = 0; i < N_CHILDREN; i++)
		child[i] = nullptr;

	setTerrainData();
}

inline TerrainPoint Oct::interpolateVertex(TerrainPoint a, TerrainPoint b, float threshold)
{
	bool flip = false;

	if (a.pos.x > b.pos.x)
		flip = true;
	else if (a.pos.x < b.pos.x)
		flip = false;

	if (a.pos.y > b.pos.y)
		flip = true;
	else if (a.pos.y < b.pos.y)
		flip = false;

	if (a.pos.z > b.pos.z)
		flip = true;
	else if (a.pos.z < b.pos.z)
		flip = false;

	if (flip) {
		TerrainPoint temp;
		temp = a;
		a = b;
		b = temp;
	}

	if (abs(a.noiseHeight - b.noiseHeight) > 0.00001)
		return TerrainPoint(a.pos + (b.pos - a.pos) / (b.noiseHeight - a.noiseHeight) * (threshold - a.noiseHeight), threshold);
	else
		return a;
}

inline void Oct::setTerrainData() {
	TerrainPoint terrainPoints[(chunkLength + 1)][(chunkLength + 1)][(chunkLength + 1)];

	for (int y = 0; y < (chunkLength + 1); y++) {
		for (int z = 0; z < (chunkLength + 1); z++) {
			for (int x = 0; x < (chunkLength + 1); x++) {
				double3 temp = double3((x - chunkLength / 2.0) / (chunkLength / 2.0) * length, (y - chunkLength / 2.0) / (chunkLength / 2.0) * length, (z - chunkLength / 2.0) / (chunkLength / 2.0) * length);				
				terrainPoints[y][z][x] = TerrainPoint(temp + pos);
			}
		}
	}

	nVertices = 0;
	nIndices = 0;

	for (int z = 0; z < chunkLength; z++) {
		for (int y = 0; y < chunkLength; y++) {
			for (int x = 0; x < chunkLength; x++) {
				TerrainPoint t[8];

				double3 cubeNormal = double3();
				double3 cubeNormals[8] = {
					double3(-1, -1, -1),
					double3(1, -1, -1),
					double3(-1, 1, -1),
					double3(1, 1, -1),
					double3(-1, -1, 1),
					double3(1, -1, 1),
					double3(-1, 1, 1),
					double3(1, 1, 1),
				};

				t[3] = terrainPoints[y + 1][z][x + 1];
				t[2] = terrainPoints[y + 1][z][x];
				t[1] = terrainPoints[y][z][x + 1];
				t[0] = terrainPoints[y][z][x];
				t[7] = terrainPoints[y + 1][z + 1][x + 1];
				t[6] = terrainPoints[y + 1][z + 1][x];
				t[5] = terrainPoints[y][z + 1][x + 1];
				t[4] = terrainPoints[y][z + 1][x];

				bool above = false;
				bool below = false;
				bool belows[8];

				for (int i = 0; i < 8; i++) {
					cubeNormal = cubeNormal + cubeNormals[i] * t[i].noiseHeight;

					belows[i] = false;
					if (t[i].noiseHeight < 0.0) {
						below = true;
						belows[i] = true;
					}
					else
						above = true;
				}

				cubeNormal = normalize(cubeNormal);

				if (above && below) {
					BYTE cubeindex = 0;
					BYTE bit = 1;
					if (t[0].noiseHeight <= 0.0) cubeindex |= 1;
					if (t[1].noiseHeight <= 0.0) cubeindex |= 2;
					if (t[2].noiseHeight <= 0.0) cubeindex |= 4;
					if (t[3].noiseHeight <= 0.0) cubeindex |= 8;
					if (t[4].noiseHeight <= 0.0) cubeindex |= 16;
					if (t[5].noiseHeight <= 0.0) cubeindex |= 32;
					if (t[6].noiseHeight <= 0.0) cubeindex |= 64;
					if (t[7].noiseHeight <= 0.0) cubeindex |= 128;

					RegularCellData cellData = regularCellData[regularCellClass[cubeindex]];

					UINT vertCount = cellData.GetVertexCount();
					UINT triCount = cellData.GetTriangleCount();

					const USHORT* vertData = regularVertexData[cubeindex];

					for (int i = 0; i < vertCount; i++) {
						TerrainPoint tp;
						
						USHORT edgeCode = vertData[i];

						USHORT v0 = (edgeCode >> 4) & 0x0F;
						USHORT v1 = edgeCode & 0x0F;

						tp = interpolateVertex(t[v0], t[v1], 0.0);

						double3 temp = tp.pos - firstCamPos;

						vertices[nVertices].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						vertices[nVertices].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

						if ((i + 1) % 3 == 0) {
							double3 normal = normalize(crossProduct(double3(vertices[nVertices].pos) - double3(vertices[nVertices - 1].pos), double3(vertices[nVertices - 2].pos) - double3(vertices[nVertices - 1].pos)));
							
							if (dotProduct(normal, cubeNormal) < 0.0)
								normal = normal * -1;
							for (int j = 0; j < 3; j++)
								vertices[nVertices - j].normal = normal.float3();
						}

						vertices[nVertices].texCoord = XMFLOAT2(float(((x - chunkLength / 2.0) / (chunkLength / 2.0)) * length), float(((z - chunkLength / 2.0) / (chunkLength / 2.0)) * length));
						vertices[nVertices].height = tp.noiseHeight;
						vertices[nVertices++].landTypeHeight = tp.noiseHeight;
					}

					// Indices
					for (int i = 0; i < triCount * 3; i++) {
						indices[nIndices++] = cellData.vertexIndex[i] + nVertices - vertCount;
					}
				}
			}
		}
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_BUFFER_DESC vertexBufferDesc;

	// Vertices
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(PlanetVertex) * nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	vertexBufferData.pSysMem = vertices;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertBuffer);

	// Indices
	ZeroMemory(&indexBufferData, sizeof(indexBufferData));
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * nIndices;

	indexBufferData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);
}

inline void Oct::update() {
    posAway = pos - camPos;
    distance = vLength(posAway);

    if (distance < length * 4.0 && length > minLength) {
        subdivide = true;
        combine = false;
    } else if (distance > length * 12.0 && length < maxLength) {
        combine = true;
        subdivide = false;
    } else {
        subdivide = false;
        combine = false;
    }

	if (child[0] == nullptr)
		draw = true;
	else
		draw = false;

	for (int i = 0; i < N_CHILDREN; i++) {
		if (child[i] != nullptr)
			child[i]->update();
	}
}

inline void Oct::addOcts(int *added, bool onlyVisible) {
	if (*added < 1) {
		if (subdivide && child[0] != nullptr) {
			for (int i = 0; i < N_CHILDREN; i++)
				child[i]->addOcts(added, onlyVisible);
		}
		else if (subdivide && (dotProduct(posAway, camDir) > 0.5 || !onlyVisible)) {
			if (nVertices > 0) {
				makeChildren();
				(*added)++;
			}
		}
	}
}

inline void Oct::removeOcts(int len) {
	if (child[0] != nullptr) {
		bool rem = true;
		for (int i = 0; i < N_CHILDREN; i++) {
			if (!child[i]->draw) {
				child[i]->removeOcts(len);
				rem = false;
			}
			else if (!child[i]->combine || child[i]->length != len) {
				rem = false;
			}
		}
		if (rem) {
			for (int i = 0; i < N_CHILDREN; i++) {
				if (child[i] != nullptr) {
					delete child[i];
					child[i] = nullptr;
				}
			}
		}
	}
}

inline void Oct::transform() {
    groundWorld = XMMatrixIdentity();
    Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
    Translation = XMMatrixTranslation(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
    translation = XMFLOAT3(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
    groundWorld = Scale * Translation;

	for (int i = 0; i < N_CHILDREN; i++) {
		if (child[i] != nullptr)
			child[i]->transform();
	}
}

inline void Oct::drawTerrain(bool drawClose) {
    if (draw) {
        if (dotProduct(posAway, camDir) > 0.5 || distance < length * 2.0) {
            if (distance < 900 && drawClose || distance >= 900 && !drawClose) {
                if (drawClose) {
                    if (length == minLength)
                        // d3d11DevCon->GSSetShader(PLANET_GS, 0, 0);
						d3d11DevCon->GSSetShader(NULL, 0, 0);
                    else
                        d3d11DevCon->GSSetShader(NULL, 0, 0);
                }

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

				/*if (distance < 900.0f)
					d3d11DevCon->RSSetState(RSCullNone); // For grass
				else
					d3d11DevCon->RSSetState(CCWcullMode);*/
				d3d11DevCon->RSSetState(RSCullNone);
                // d3d11DevCon->RSSetState(Wireframe);
                d3d11DevCon->DrawIndexed(nIndices, 0, 0);
            }
        }
    } else {
        for (int i = 0; i < N_CHILDREN; i++) {
			if (child[i] != nullptr)
				child[i]->drawTerrain(drawClose);
        }
    }
}

inline void Oct::makeChildren() {
    for (int i = 0; i < N_CHILDREN; i++) {
		double3 childPos;

        if (i == 0) {
            childPos = double3(pos.x - 0.5*length, pos.y - 0.5*length, pos.z - 0.5*length);
        } else if (i == 1) {
            childPos = double3(pos.x + 0.5*length, pos.y - 0.5*length, pos.z - 0.5*length);
        } else if (i == 2) {
            childPos = double3(pos.x + 0.5*length, pos.y - 0.5*length, pos.z + 0.5*length);
        } else if (i == 3) {
            childPos = double3(pos.x - 0.5*length, pos.y - 0.5*length, pos.z + 0.5*length);
        } else if (i == 4) {
			childPos = double3(pos.x - 0.5*length, pos.y + 0.5*length, pos.z - 0.5*length);
		} else if (i == 5) {
			childPos = double3(pos.x + 0.5*length, pos.y + 0.5*length, pos.z - 0.5*length);
		} else if (i == 6) {
			childPos = double3(pos.x + 0.5*length, pos.y + 0.5*length, pos.z + 0.5*length);
		} else if (i == 7) {
			childPos = double3(pos.x - 0.5*length, pos.y + 0.5*length, pos.z + 0.5*length);
		}
        
        child[i] = new Oct(childPos, length / 2.0);
    }
}

inline Oct::~Oct() {
	if (vertBuffer != nullptr)
		vertBuffer->Release();
	if (indexBuffer != nullptr)
		indexBuffer->Release();
    for (int i = 0; i < N_CHILDREN; i++) {
		if (child[i] != nullptr) {
			delete child[i];
			child[i] = nullptr;
		}
    }
}

#endif MYGAME_64_OCTTREE_H
