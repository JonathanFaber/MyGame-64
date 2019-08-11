//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_QUADTREE_H
#define MYGAME_64_QUADTREE_H

#include "includes.h"
#include "terrain.h"

#define N_CHILDREN 8

struct InputData
{
	XMFLOAT3 position;
	XMFLOAT3 velocity;
	XMFLOAT3 initialVelocity;
};

struct OutputData
{
	XMFLOAT3 position;
	XMFLOAT3 velocity;
};

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

	TerrainPoint terrainPoint;
	TerrainPoint terrainPoints[(chunkLength + 1)][(chunkLength + 1)][(chunkLength + 1)];

    PlanetVertex vertices[(chunkLength)*(chunkLength)*(chunkLength)*8];
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

	const int NUM_PARTICLES = 10000;
	D3D11_SUBRESOURCE_DATA inputBufferData;

	InputData particles[NUM_PARTICLES];

	for (int i = 0; i < NUM_PARTICLES; i++) {
		particles[i].initialVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
		particles[i].velocity = XMFLOAT3(0.0f, 5.0f, 0.0f);
		particles[i].position = XMFLOAT3(1.0f, 0.0f, 10.0f);
	}

	ZeroMemory(&inputBufferData, sizeof(inputBufferData));
	inputBufferData.pSysMem = particles;

	computeShader.setData(&inputBufferData, sizeof(InputData), sizeof(OutputData), NUM_PARTICLES);

	// Enable Compute Shader
	OutputData* dataView = reinterpret_cast<OutputData*>(computeShader.runComputation(int3(1024, 1, 1)));

	if (dataView[0].position.y == 5.0f) {
		setVertexData();
		setIndexData();
	}
}

bool isSame(TerrainPoint a, TerrainPoint b) {
	return (a.landHeight < 0.0 && b.landHeight < 0.0) || (a.landHeight >= 0.0 && b.landHeight >= 0.0);
}

inline void Quad::setVertexData() {
	for (int y = 0; y < (chunkLength + 1); y++) {
		for (int z = 0; z < (chunkLength + 1); z++) {
			for (int x = 0; x < (chunkLength + 1); x++) {
				double3 temp = double3((x - chunkLength / 2.0) / (chunkLength / 2.0) * length, (y - chunkLength / 2.0) / (chunkLength / 2.0) * length, (z - chunkLength / 2.0) / (chunkLength / 2.0) * length);

				temp = temp + pos;

				terrainPoint.generateTerrainPoint(temp);
				terrainPoints[y][z][x] = terrainPoint;				
			}
		}
	}

	counter = 0;
	for (int y = 0; y < chunkLength; y++) {
		for (int z = 0; z < chunkLength; z++) {
			for (int x = 0; x < chunkLength; x++) {
				TerrainPoint t[8];

				t[0] = terrainPoints[y][z][x];
				t[4] = terrainPoints[y][z][x + 1];
				t[5] = terrainPoints[y][z + 1][x + 1];
				t[1] = terrainPoints[y][z + 1][x];
				t[2] = terrainPoints[y + 1][z][x];
				t[6] = terrainPoints[y + 1][z][x + 1];
				t[7] = terrainPoints[y + 1][z + 1][x + 1];
				t[3] = terrainPoints[y + 1][z + 1][x];

				bool above = false;
				bool below = false;
				bool belows[8];

				for (int i = 0; i < 8; i++) {
					belows[i] = false;
					if (t[i].landHeight < 0.0) {
						below = true;
						belows[i] = true;
					}
					else
						above = true;
				}

				if (above && below) {
					/*for (int z1 = 0; z1 < 2; z1++) {
						for (int y1 = 0; y1 < 2; y1++) {
							for (int x1 = 0; x1 < 2; x1++) {
								int idx[4];
								idx[0] = x1 * 4 + y1 * 2 + z1;
								idx[1] = ((x1 + 1) % 2) * 4 + ((y1) % 2) * 2 + ((z1) % 2);
								idx[2] = ((x1) % 2) * 4 + ((y1 + 1) % 2) * 2 + ((z1) % 2);
								idx[3] = ((x1) % 2) * 4 + ((y1) % 2) * 2 + ((z1 + 1) % 2);

								if (!isSame(t[idx[0]], t[idx[1]]))
									;
							}
						}
					}*/

					for (int i = 0; i < 8; i++) {
						double3 temp = t[i].pos - firstCamPos;

						vertices[counter].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						vertices[counter].normal = i < 4 ? XMFLOAT3(0.0f, 1.0f, 0.0f) : XMFLOAT3(1.0f, 0.0f, 0.0f);
						vertices[counter].texCoord = XMFLOAT2(float(((x - chunkLength / 2.0) / (chunkLength / 2.0)) * length), float(((z - chunkLength / 2.0) / (chunkLength / 2.0)) * length));

						vertices[counter].height = t[i].landHeight;
						vertices[counter++].landTypeHeight = t[i].landHeight;
					}
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
	DWORD idx[8];
	counter = 0;
	// might need to do order 012032 for gs shader
	for (int i = 0; i < nVertices / 8; i++) {
					// first square
					indices[counter++] = 8 * i;
					indices[counter++] = 8 * i + 4;
					indices[counter++] = 8 * i + 5;
					indices[counter++] = 8 * i;
					indices[counter++] = 8 * i + 5;
					indices[counter++] = 8 * i + 1;
					// second
					indices[counter++] = 8 * i;
					indices[counter++] = 8 * i + 4;
					indices[counter++] = 8 * i + 6;
					indices[counter++] = 8 * i;
					indices[counter++] = 8 * i + 6;
					indices[counter++] = 8 * i + 2;
					// third
					indices[counter++] = 8 * i + 1;
					indices[counter++] = 8 * i;
					indices[counter++] = 8 * i + 2;
					indices[counter++] = 8 * i + 1;
					indices[counter++] = 8 * i + 2;
					indices[counter++] = 8 * i + 3;
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

    if (distance < length * 2.0 && length > minLength) {
        subdivide = true;
        combine = false;
    } else if (distance > length * 6.0 && length < maxLength) {
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
				delete child[i];
				child[i] = nullptr;
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
                //d3d11DevCon->RSSetState(Wireframe);
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

        // Next apply height
		double3 tempNormal = normalize(child[i]->pos);
        terrainPoint.generateTerrainPoint(tempNormal);

        child[i]->pos = child[i]->pos;

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
        delete child[i];
        child[i] = nullptr;
    }
}



#endif MYGAME_64_QUADTREE_H
