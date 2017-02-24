//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_QUADTREE_H
#define MYGAME_64_QUADTREE_H

#include "includes.h"
#include "shaders.h"
#include "terrain.h"

class Quad {
public:
    double3 pos;
    double3 posSquare;
    double3 posAway;
    double distance;
    double length;
    bool subdivide;
    bool combine;
    bool draw;
    char side;
    double squarePosSide;
    Quad *child[4];

    Quad() {}
    Quad(double3 pos, double3 posSquare, double length, char side, double squarePosSide);
    ~Quad();

    PlanetVertex verticesFinal[(chunkLength + 1)*(chunkLength + 1)];
    XMFLOAT3 positions[chunkLength + 3][chunkLength + 3];
    double3 firstCamPos;
    bool terrainCalculated;
    bool firstUpdate;

    void create();
    void update();
    void transform(double3 camPos);
    void setChunkData();
    void drawTerrain(bool drawClose);
    void makeChildren();

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
};

#endif MYGAME_64_QUADTREE_H
