//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_QUADTREE_H
#define MYGAME_64_QUADTREE_H

#include "includes.h"
#include "terrain.h"

class Quad {
public:
    Quad() {}
    Quad(double3 pos, double length);
    ~Quad();

    XMFLOAT3 positions[chunkLength + 3][chunkLength + 3];
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
	Quad *child[4];

    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertBuffer;
    XMMATRIX groundWorld;
    XMFLOAT3 translation;

    PlanetVertex vertices[(chunkLength + 1)*(chunkLength + 1)];
    DWORD indices[chunkLength*chunkLength * 6];
	int nIndices = chunkLength * chunkLength * 6;

    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA indexBufferData;
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA initialVertexBufferData;

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
    for (int i = 0; i < 4; i++)
        child[i] = nullptr;

    draw = true;
    subdivide = false;
    combine = false;

	setVertexData();
	setIndexData();
}

inline void Quad::setVertexData() {
	counter = 0;
	for (int z = 0; z < (chunkLength + 3); z++) {
		for (int x = 0; x < (chunkLength + 3); x++) {
			double3 temp = double3((x - 1 - chunkLength / 2) / 16.0 * length, 0.0, (z - 1 - chunkLength / 2) / 16.0 * length);

			temp = temp + pos;

			double3 tempNormal = normalize(temp);
			terrainPoint.generateTerrainPoint(tempNormal);

			// temp = temp + terrainPoint.terrain;
			temp = temp - firstCamPos;

			positions[x][z] = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));

			if (x != 0 && z != 0 && x != chunkLength + 2 && z != chunkLength + 2) {
				vertices[counter].pos = positions[x][z];
				vertices[counter].texCoord = XMFLOAT2(float(((x - chunkLength / 2) / 16.0) * length), float(((z - chunkLength / 2) / 16.0) * length));

				vertices[counter].height = terrainPoint.height_f;
				vertices[counter++].landTypeHeight = terrainPoint.landTypeHeight_f;
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

				// if (dotProduct(f_normals[i][x][z], XMFLOAT3(pos.x, pos.y, pos.z)) < 0.0f) {
				if (f_normals[i][x][z].y < 0.0f) {
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

			vertices[counter++].normal = normals[x - 1][z - 1];
		}
	}

	ZeroMemory(&initialVertexBufferData, sizeof(initialVertexBufferData));
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	initialVertexBufferData.pSysMem = vertices;
	hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &initialVertexBufferData, &vertBuffer);
}

inline void Quad::setIndexData() {
	counter = 0;
	// might need to do order 012032 for gs shader
	for (int x = 0; x < chunkLength; x++) {
		for (int z = 0; z < chunkLength; z++) {
			if (true) {
				indices[counter++] = x + z * (chunkLength + 1); // 0
				indices[counter++] = x + 1 + z * (chunkLength + 1); // 1
				indices[counter++] = x + (chunkLength + 2) + z * (chunkLength + 1); // 2
				indices[counter++] = x + z * (chunkLength + 1); // 0
				indices[counter++] = x + (chunkLength + 2) + z * (chunkLength + 1); // 2
				indices[counter++] = x + (chunkLength + 1) + z * (chunkLength + 1); // 3
			}
		}
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

	for (int i = 0; i < 4; i++) {
		if (child[i] != nullptr)
			child[i]->update();
	}
}

inline void Quad::addQuads(int *added, bool onlyVisible) {
	if (*added < 1) {
		if (subdivide && child[0] != nullptr) {
			for (int i = 0; i < 4; i++)
				child[i]->addQuads(added, onlyVisible);
		}
		else if (subdivide && (dotProduct(posAway, camDir) > 0.5 || !onlyVisible)) {
			makeChildren();
			(*added)++;
		}
	}
}

inline void Quad::removeQuads(int len) {
	if (child[0] != nullptr) {
		bool rem = true;
		for (int i = 0; i < 4; i++) {
			if (!child[i]->draw) {
				child[i]->removeQuads(len);
				rem = false;
			}
			else if (!child[i]->combine || child[i]->length != len) {
				rem = false;
			}
		}
		if (rem) {
			for (int i = 0; i < 4; i++) {
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

	for (int i = 0; i < 4; i++) {
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
                        d3d11DevCon->GSSetShader(PLANET_GS, 0, 0);
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

				if (distance < 900.0f)
					d3d11DevCon->RSSetState(RSCullNone); // For grass
				else
					d3d11DevCon->RSSetState(CCWcullMode);
                // d3d11DevCon->RSSetState(Wireframe);
                d3d11DevCon->DrawIndexed(nIndices, 0, 0);
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
			if (child[i] != nullptr)
				child[i]->drawTerrain(drawClose);
        }
    }
}

inline void Quad::makeChildren() {
    for (int i = 0; i < 4; i++) {
        child[i] = new Quad();

        if (i == 0) {
            child[i]->pos = double3(pos.x - 0.5*length, maxLength, pos.z - 0.5*length);
        } else if (i == 1) {
            child[i]->pos = double3(pos.x + 0.5*length, maxLength, pos.z - 0.5*length);
        } else if (i == 2) {
            child[i]->pos = double3(pos.x + 0.5*length, maxLength, pos.z + 0.5*length);
        } else if (i == 3) {
            child[i]->pos = double3(pos.x - 0.5*length, maxLength, pos.z + 0.5*length);
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
    vertBuffer->Release();
    indexBuffer->Release();
    for (int i = 0; i < 4; i++) {
        delete child[i];
        child[i] = nullptr;
    }
}



#endif MYGAME_64_QUADTREE_H
