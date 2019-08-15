//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_QUADTREE_H
#define MYGAME_64_QUADTREE_H

#include "includes.h"
#include "terrain.h"
#include "Transvoxel.h"

#define N_CHILDREN 8

class Quad {
public:
    Quad() {}
    Quad(double3 pos, double length);
    ~Quad();

    double3 firstCamPos;

    void update();
    void transform();
    void drawTerrain(bool drawClose);
	void addQuads(int *added, bool onlyVisible);
	void removeQuads(int len);

private:
	double3 pos;
	double3 posAway;
	double distance;
	double length;
	bool subdivide;
	bool combine;
	bool draw;
	Quad *child[N_CHILDREN];

    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertBuffer;
    XMMATRIX groundWorld;
    XMFLOAT3 translation;

    PlanetVertex vertices[(chunkLength)*(chunkLength)*(chunkLength)*12];
    DWORD indices[chunkLength*chunkLength*chunkLength * 6 * 3];
	int nVertices = 0;
	int nIndices = chunkLength * chunkLength * chunkLength * 6 * 3;

    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA indexBufferData;
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexBufferData;

	void create();
	void setVertexData();
	void setIndexData();
	void makeChildren();
};

inline Quad::Quad(double3 pos, double length) {
    this->pos = pos;
    this->length = length;
    create();
}

inline void Quad::create() {
    for (int i = 0; i < N_CHILDREN; i++)
        child[i] = nullptr;

    draw = true;
    subdivide = false;
    combine = false;

	setVertexData();
	setIndexData();
}

bool isSame(TerrainPoint a, TerrainPoint b) {
	return (a.noiseHeight < 0.0 && b.noiseHeight < 0.0) || (a.noiseHeight >= 0.0 && b.noiseHeight >= 0.0);
}

bool comparePoints(double3 a, double3 b)
{
	if (a.x < b.x)
		return true;
	else if (a.x > b.x)
		return false;

	if (a.y < b.y)
		return true;
	else if (a.y > b.y)
		return false;

	if (a.z < b.z)
		return true;
	else if (a.z > b.z)
		return false;

	return false;
}

TerrainPoint vertInter(TerrainPoint a, TerrainPoint b, float threshold)
{
	if (comparePoints(b.pos, a.pos))
	{
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

//TerrainPoint vertInter(TerrainPoint a, TerrainPoint b, double threshold) {
//	if (abs(threshold - a.noiseHeight) < 0.00001)
//		return a;
//	if (abs(threshold - b.noiseHeight) < 0.00001)
//		return b;
//	if (abs(a.noiseHeight - b.noiseHeight) < 0.00001)
//		return a;
//
//	return TerrainPoint(a.pos + (b.pos - a.pos) * (threshold - a.noiseHeight) / (b.noiseHeight - a.noiseHeight), threshold);
//}

inline void Quad::setVertexData() {
	TerrainPoint terrainPoints[(chunkLength + 1)][(chunkLength + 1)][(chunkLength + 1)];

	for (int y = 0; y < (chunkLength + 1); y++) {
		for (int z = 0; z < (chunkLength + 1); z++) {
			for (int x = 0; x < (chunkLength + 1); x++) {
				double3 temp = double3((x - chunkLength / 2.0) / (chunkLength / 2.0) * length, (y - chunkLength / 2.0) / (chunkLength / 2.0) * length, (z - chunkLength / 2.0) / (chunkLength / 2.0) * length);				
				terrainPoints[y][z][x] = TerrainPoint(temp + pos);
			}
		}
	}

	counter = 0;
	for (int z = 0; z < chunkLength; z++) {
		for (int y = 0; y < chunkLength; y++) {
			for (int x = 0; x < chunkLength; x++) {
				TerrainPoint t[8];

				double3 cubeNormal = double3();
				double3 cubeNormals[8] = {
					double3(-1, -1, 1),
					double3(1, -1, 1),
					double3(1, -1, -1),
					double3(-1, -1, 1),
					double3(-1, 1, 1),
					double3(1, 1, 1),
					double3(1, 1, -1),
					double3(-1, 1, -1),
				};

				t[3] = terrainPoints[y][z][x];
				t[2] = terrainPoints[y][z][x + 1];
				t[1] = terrainPoints[y][z + 1][x + 1];
				t[0] = terrainPoints[y][z + 1][x];
				t[7] = terrainPoints[y + 1][z][x];
				t[6] = terrainPoints[y + 1][z][x + 1];
				t[5] = terrainPoints[y + 1][z + 1][x + 1];
				t[4] = terrainPoints[y + 1][z + 1][x];

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

					INT8* edges = edgeTable[cubeindex];

					RegularCellData cellData = regularCellData[regularCellClass[cubeindex]];

					UINT vertCount = cellData.GetVertexCount();
					UINT triCount = cellData.GetTriangleCount();

					const USHORT* vertData = regularVertexData[cubeindex];

					for (int i = 0; i < 12; i++) {
						TerrainPoint tp;
						/*
						USHORT edgeCode = vertData[i];

						USHORT v0 = (edgeCode >> 4) & 0x0F;
						USHORT v1 = edgeCode & 0x0F;

						long d0 = corner[v0]; long d1 = corner[v1];
						long t = (d1 << 8) / (d1 - d0);

						if ((t & 0x00FF) != 0) {  // Vertex lies in the interior of the edge. 
							long u = 0x0100 - t;  Q = t * P0 + u * P1;
						}
						else if (t == 0) {  // Vertex lies at the higher-numbered endpoint. 
							if (v1 == 7) {   
								// This cell owns the vertex.  
							}  else  {   
								// Try to reuse corner vertex from a preceding cell.  
							} 
						}
						else {
							// Vertex lies at the lower-numbered endpoint.  // Always try to reuse corner vertex from a preceding cell
						}
						*/

						if (edges[i] == -1) break;
						if (edges[i] == 0) {
							tp = vertInter(t[0], t[1], 0.0);
						}
						else if (edges[i] == 1) {
							tp = vertInter(t[2], t[1], 0.0);
						}
						else if (edges[i] == 2) {
							tp = vertInter(t[3], t[2], 0.0);
						}
						else if (edges[i] == 3) {
							tp = vertInter(t[3], t[0], 0.0);
						}
						else if (edges[i] == 4) {
							tp = vertInter(t[4], t[5], 0.0);
						}
						else if (edges[i] == 5) {
							tp = vertInter(t[6], t[5], 0.0);
						}
						else if (edges[i] == 6) {
							tp = vertInter(t[7], t[6], 0.0);
						}
						else if (edges[i] == 7) {
							tp = vertInter(t[7], t[4], 0.0);
						}
						else if (edges[i] == 8) {
							tp = vertInter(t[0], t[4], 0.0);
						}
						else if (edges[i] == 9) {
							tp = vertInter(t[1], t[5], 0.0);
						}
						else if (edges[i] == 10) {
							tp = vertInter(t[2], t[6], 0.0);
						}
						else if (edges[i] == 11) {
							tp = vertInter(t[3], t[7], 0.0);
						}

						double3 temp = tp.pos - firstCamPos;

						vertices[counter].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						vertices[counter].normal = XMFLOAT3(0.0f, 0.0f, 0.0f);

						if ((i + 1) % 3 == 0) {
							double3 normal = normalize(crossProduct(double3(vertices[counter].pos) - double3(vertices[counter - 1].pos), double3(vertices[counter - 2].pos) - double3(vertices[counter - 1].pos)));
							
							if (dotProduct(normal, cubeNormal) < 0.0)
								normal = normal * -1;
							for (int j = 0; j < 3; j++)
								vertices[counter - j].normal = normal.float3();
						}

						vertices[counter].texCoord = XMFLOAT2(float(((x - chunkLength / 2.0) / (chunkLength / 2.0)) * length), float(((z - chunkLength / 2.0) / (chunkLength / 2.0)) * length));
						vertices[counter].height = tp.noiseHeight;
						vertices[counter++].landTypeHeight = tp.noiseHeight;
					}
					/*
					for (int i = 0; i < 8; i++) {
						double3 temp = t[i].pos - firstCamPos;

						vertices[counter].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						vertices[counter].normal = i < 4 ? XMFLOAT3(0.0f, 1.0f, 0.0f) : XMFLOAT3(1.0f, 0.0f, 0.0f);
						vertices[counter].texCoord = XMFLOAT2(float(((x - chunkLength / 2.0) / (chunkLength / 2.0)) * length), float(((z - chunkLength / 2.0) / (chunkLength / 2.0)) * length));

						vertices[counter].height = t[i].noiseHeight;
						vertices[counter++].landTypeHeight = t[i].noiseHeight;
					}*/
				}
			}
		}
	}
	nVertices = counter;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(PlanetVertex) * nVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	vertexBufferData.pSysMem = vertices;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertBuffer);
}

inline void Quad::setIndexData() {
	counter = 0;
	// might need to do order 012032 for gs shader
	for (int i = 0; i < nVertices; i++) {
		// first square
		indices[counter++] = i;
	}
	nIndices = counter;

	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * nIndices;

	indexBufferData.pSysMem = indices;
	d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);
}

inline void Quad::update() {
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

inline void Quad::addQuads(int *added, bool onlyVisible) {
	if (*added < 1) {
		if (subdivide && child[0] != nullptr) {
			for (int i = 0; i < N_CHILDREN; i++)
				child[i]->addQuads(added, onlyVisible);
		}
		else if (subdivide && (dotProduct(posAway, camDir) > 0.5 || !onlyVisible)) {
			if (nVertices > 0) {
				makeChildren();
				(*added)++;
			}
		}
	}
}

inline void Quad::removeQuads(int len) {
	if (child[0] != nullptr) {
		bool rem = true;
		for (int i = 0; i < N_CHILDREN; i++) {
			if (!child[i]->draw) {
				child[i]->removeQuads(len);
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

inline void Quad::transform() {
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

inline void Quad::drawTerrain(bool drawClose) {
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

inline void Quad::makeChildren() {
    for (int i = 0; i < N_CHILDREN; i++) {
        child[i] = new Quad();

        if (i == 0) {
            child[i]->pos = double3(pos.x - 0.5*length, pos.y - 0.5*length, pos.z - 0.5*length);
        } else if (i == 1) {
            child[i]->pos = double3(pos.x + 0.5*length, pos.y - 0.5*length, pos.z - 0.5*length);
        } else if (i == 2) {
            child[i]->pos = double3(pos.x + 0.5*length, pos.y - 0.5*length, pos.z + 0.5*length);
        } else if (i == 3) {
            child[i]->pos = double3(pos.x - 0.5*length, pos.y - 0.5*length, pos.z + 0.5*length);
        } else if (i == 4) {
			child[i]->pos = double3(pos.x - 0.5*length, pos.y + 0.5*length, pos.z - 0.5*length);
		} else if (i == 5) {
			child[i]->pos = double3(pos.x + 0.5*length, pos.y + 0.5*length, pos.z - 0.5*length);
		} else if (i == 6) {
			child[i]->pos = double3(pos.x + 0.5*length, pos.y + 0.5*length, pos.z + 0.5*length);
		} else if (i == 7) {
			child[i]->pos = double3(pos.x - 0.5*length, pos.y + 0.5*length, pos.z + 0.5*length);
		}
        
        child[i]->length = length / 2.0;
        child[i]->firstCamPos = camPos;

        child[i]->subdivide = false;
        child[i]->combine = false;
        child[i]->draw = true;

        child[i]->create();
    }
}

inline Quad::~Quad() {
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



#endif MYGAME_64_QUADTREE_H
