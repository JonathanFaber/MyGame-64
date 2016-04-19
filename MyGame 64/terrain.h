#include "includes.h"
#include "planet.h"

#ifndef __TERRAIN_H_INCLUDED__
#define __TERRAIN_H_INCLUDED__ 

ID3D11Buffer* terrainIndexBuffer_highQ;
ID3D11Buffer* terrainVertBuffer_highQ[7][7];
ID3D11Buffer* terrainIndexBuffer_mediumQ;
ID3D11Buffer* terrainVertBuffer_mediumQ[44];
vec2 terrain_mediumQ_Pos[44];
XMMATRIX groundWorld_highQ;
XMMATRIX groundWorld_mediumQ;

const int BigChunkSize = 256;

float points[5][5][BigChunkSize + 1][BigChunkSize + 1];
float smoothPoints[5][5][BigChunkSize + 1][BigChunkSize + 1];
float allPoints[BigChunkSize * 5 + 1][BigChunkSize * 5 + 1];
float smoothPointsAvg[5][5];

int random = 5;
float randomH;

bool done[BigChunkSize + 1][BigChunkSize + 1];
int length = BigChunkSize;
float amplitude;

bool BigChunkDone[5][5];

vec2 regionPos;
vec2 region;
vec2 smallRegionPos;
vec2 smallRegion;
vec2 bigRegionPos;
vec2 bigRegion;

bool change = false;
bool smallChange = false;
int seed = 1;
int randSeed[1001][1001];

Vertex terrainVertices_highQ[7][7][33 * 33];
DWORD terrainIndices_highQ[6 * 32 * 32];
Vertex terrainVertices_mediumQ[44][17 * 17];
DWORD terrainIndices_mediumQ[6 * 16 * 16];

XMFLOAT3 f_normals[2][BigChunkSize * 5][BigChunkSize * 5];
XMFLOAT3 V;
XMFLOAT3 W;
XMFLOAT3 normals[BigChunkSize * 5 + 1][BigChunkSize * 5 + 1];

D3D11_BUFFER_DESC terrainIndexBufferDesc_highQ;
D3D11_SUBRESOURCE_DATA terrainIndexBufferData_highQ;
D3D11_BUFFER_DESC terrainVertexBufferDesc_highQ;
D3D11_SUBRESOURCE_DATA terrainVertexBufferData_highQ[7][7];

D3D11_BUFFER_DESC terrainIndexBufferDesc_mediumQ;
D3D11_SUBRESOURCE_DATA terrainIndexBufferData_mediumQ;
D3D11_BUFFER_DESC terrainVertexBufferDesc_mediumQ;
D3D11_SUBRESOURCE_DATA terrainVertexBufferData_mediumQ[44];


//////////Terrain//////////
void calculateTerrain1() {
	for (int m = 0; m < 5; m++) {
		for (int n = 0; n < 5; n++) {

			if (floor(float(region.x + m + region.z + n) / 2.0f) == float((region.x + m + region.z + n) / 2.0f)) { // Even number
				BigChunkDone[m][n] = true;

				for (int z = 0; z < BigChunkSize + 1; z++) {
					for (int x = 0; x < BigChunkSize + 1; x++) {
						points[m][n][x][z] = 0.0f;
						done[x][z] = false;
					}
				}

				srand(randSeed[500 + m - 2][500 + n - 2]);

				amplitude = 140.5f;

				//////////////////////////////////////////
				for (int i = 0; i < 8; i++) {
					for (int x = 0; x < BigChunkSize / length; x++) {
						for (int z = 0; z < BigChunkSize / length; z++) {
							//Middle
							randomH = float((rand() % 10000) - 5000) / 5000.0f;
							points[m][n][length / 2 + x*length][length / 2 + z*length] = (points[m][n][x*length][z*length] + points[m][n][x*length][(z + 1)*length] + points[m][n][(x + 1)*length][(z + 1)*length] + points[m][n][(x + 1)*length][z*length]) / 4 + randomH*amplitude;
						}
					}

					for (int x = 0; x < BigChunkSize / length; x++) {
						for (int z = 0; z < BigChunkSize / length; z++) {
							if (done[x*length][length / 2 + z*length] == false) {	//west
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (x != 0) {
									points[m][n][x*length][length / 2 + z*length] = (points[m][n][x*length - length / 2][length / 2 + z*length] + points[m][n][x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][x*length][z*length]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][x*length][length / 2 + z*length] = (points[m][n][x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][x*length][z*length]) / 3 + randomH*amplitude;
								}

								done[x*length][length / 2 + z*length] = true;
							}
							if (done[length / 2 + x*length][length + z*length] == false) {	//north
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (z != BigChunkSize / length - 1) {
									points[m][n][length / 2 + x*length][length + z*length] = (points[m][n][x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length + length] + points[m][n][length + x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][length / 2 + x*length][length + z*length] = (points[m][n][x*length][length + z*length] + points[m][n][length + x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length]) / 3 + randomH*amplitude;
								}


								done[length / 2 + x*length][length + z*length] = true;
							}
							if (done[length + x*length][length / 2 + z*length] == false) {	//east
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (x != BigChunkSize / length - 1) {
									points[m][n][length + x*length][length / 2 + z*length] = (points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][length + z*length] + points[m][n][length + x*length + length / 2][length / 2 + z*length] + points[m][n][length + x*length][z*length]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][length + x*length][length / 2 + z*length] = (points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][length + z*length] + points[m][n][length + x*length][z*length]) / 3 + randomH*amplitude;
								}

								done[length + x*length][length / 2 + z*length] = true;
							}
							if (done[length / 2 + x*length][z*length] == false) {	//south
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (z != 0) {
									points[m][n][length / 2 + x*length][z*length] = (points[m][n][x*length][z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][z*length] + points[m][n][length / 2 + x*length][z*length - length / 2]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][length / 2 + x*length][z*length] = (points[m][n][x*length][z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][z*length]) / 3 + randomH*amplitude;
								}

								done[length / 2 + x*length][z*length] = true;
							}
						}
					}

					amplitude *= 0.5f;
					length *= 0.5;
				}
				///////////////////////////
				length = BigChunkSize;

				smoothPointsAvg[m][n] = 0.0f;

				for (int z = 0; z < (BigChunkSize+1); z++) {
					for (int x = 0; x < (BigChunkSize+1); x++) {
						if (x != 0 && z != 0 && x != BigChunkSize && z != BigChunkSize) {
							smoothPoints[m][n][x][z] = (points[m][n][x - 1][z] + points[m][n][x][z + 1] + points[m][n][x + 1][z] + points[m][n][x][z - 1]) / 4;
						}
						else if (x == 0 && z != 0 && z != BigChunkSize) {
							smoothPoints[m][n][x][z] = (points[m][n][x][z + 1] + points[m][n][x + 1][z] + points[m][n][x][z - 1]) / 3;
						}
						else if (z == BigChunkSize && x != 0 && x != BigChunkSize) {
							smoothPoints[m][n][x][z] = (points[m][n][x - 1][z] + points[m][n][x + 1][z] + points[m][n][x][z - 1]) / 3;
						}
						else if (x == BigChunkSize && z != 0 && z != BigChunkSize) {
							smoothPoints[m][n][x][z] = (points[m][n][x][z + 1] + points[m][n][x - 1][z] + points[m][n][x][z - 1]) / 3;
						}
						else if (z == 0 && x != 0 && x != BigChunkSize) {
							smoothPoints[m][n][x][z] = (points[m][n][x][z + 1] + points[m][n][x + 1][z] + points[m][n][x - 1][z]) / 3;
						}
						smoothPointsAvg[m][n] += smoothPoints[m][n][x][z];
					}
				}

				smoothPointsAvg[m][n] = smoothPointsAvg[m][n] / ((BigChunkSize+1) * (BigChunkSize+1));
			}
			else {
				BigChunkDone[m][n] = false;
			}
		}
	}
}

void calculateTerrain2() {
	for (int m = 1; m < 4; m++) {
		for (int n = 1; n < 4; n++) {

			if (BigChunkDone[m][n] == false) { // Odd number
				BigChunkDone[m][n] = true;

				for (int z = 0; z < BigChunkSize + 1; z++) {
					for (int x = 0; x < BigChunkSize + 1; x++) {
						points[m][n][x][z] = 0.0f;
						done[x][z] = false;
					}
				}

				srand(randSeed[500 + m - 2][500 + n - 2]);

				for (int x = 0; x < (BigChunkSize+1); x++) {	// North
					points[m][n][x][BigChunkSize] = points[m][n+1][x][0];
					done[x][BigChunkSize] = true;
				}
				for (int z = 0; z < (BigChunkSize+1); z++) {	// East
					points[m][n][BigChunkSize][z] = points[m+1][n][0][z];
					done[BigChunkSize][z] = true;
				}
				for (int x = 0; x < (BigChunkSize+1); x++) {	// South
					points[m][n][x][0] = points[m][n-1][x][BigChunkSize];
					done[x][0] = true;
				}
				for (int z = 0; z < (BigChunkSize+1); z++) {	// West
					points[m][n][0][z] = points[m-1][n][BigChunkSize][z];
					done[0][z] = true;
				}

				amplitude = 140.5f;

				//////////////////////////////////////////
				for (int i = 0; i < 8; i++) {
					for (int x = 0; x < BigChunkSize / length; x++) {
						for (int z = 0; z < BigChunkSize / length; z++) {
							//Middle
							randomH = float((rand() % 10000) - 5000) / 5000.0f;
							points[m][n][length / 2 + x*length][length / 2 + z*length] = (points[m][n][x*length][z*length] + points[m][n][x*length][(z + 1)*length] + points[m][n][(x + 1)*length][(z + 1)*length] + points[m][n][(x + 1)*length][z*length]) / 4 + randomH*amplitude;
						}
					}

					for (int x = 0; x < BigChunkSize / length; x++) {
						for (int z = 0; z < BigChunkSize / length; z++) {
							if (done[x*length][length / 2 + z*length] == false) {	//west
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (x != 0) {
									points[m][n][x*length][length / 2 + z*length] = (points[m][n][x*length - length / 2][length / 2 + z*length] + points[m][n][x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][x*length][z*length]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][x*length][length / 2 + z*length] = (points[m][n][x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][x*length][z*length]) / 3 + randomH*amplitude;
								}

								done[x*length][length / 2 + z*length] = true;
							}
							if (done[length / 2 + x*length][length + z*length] == false) {	//north
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (z != BigChunkSize / length - 1) {
									points[m][n][length / 2 + x*length][length + z*length] = (points[m][n][x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length + length] + points[m][n][length + x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][length / 2 + x*length][length + z*length] = (points[m][n][x*length][length + z*length] + points[m][n][length + x*length][length + z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length]) / 3 + randomH*amplitude;
								}


								done[length / 2 + x*length][length + z*length] = true;
							}
							if (done[length + x*length][length / 2 + z*length] == false) {	//east
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (x != BigChunkSize / length - 1) {
									points[m][n][length + x*length][length / 2 + z*length] = (points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][length + z*length] + points[m][n][length + x*length + length / 2][length / 2 + z*length] + points[m][n][length + x*length][z*length]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][length + x*length][length / 2 + z*length] = (points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][length + z*length] + points[m][n][length + x*length][z*length]) / 3 + randomH*amplitude;
								}

								done[length + x*length][length / 2 + z*length] = true;
							}
							if (done[length / 2 + x*length][z*length] == false) {	//south
								randomH = float((rand() % 10000) - 5000) / 5000.0f;
								if (z != 0) {
									points[m][n][length / 2 + x*length][z*length] = (points[m][n][x*length][z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][z*length] + points[m][n][length / 2 + x*length][z*length - length / 2]) / 4 + randomH*amplitude;
								}
								else {
									points[m][n][length / 2 + x*length][z*length] = (points[m][n][x*length][z*length] + points[m][n][length / 2 + x*length][length / 2 + z*length] + points[m][n][length + x*length][z*length]) / 3 + randomH*amplitude;
								}

								done[length / 2 + x*length][z*length] = true;
							}
						}
					}

					amplitude *= 0.5f;
					length *= 0.5;
				}
				///////////////////////////
				length = BigChunkSize;
			}
		}
	}


	for (int m = 0; m < 5; m++) {
		for (int n = 0; n < 5; n++) {
			smoothPointsAvg[m][n] = 0.0f;

			for (int z = 0; z < (BigChunkSize+1); z++) {
				for (int x = 0; x < (BigChunkSize+1); x++) {
					if (x != 0 && z != 0 && x != BigChunkSize && z != BigChunkSize) {
						smoothPoints[m][n][x][z] = (points[m][n][x - 1][z] + points[m][n][x][z + 1] + points[m][n][x + 1][z] + points[m][n][x][z - 1]) / 4;
					}
					else if (x == 0 && z != 0 && z != BigChunkSize) {
						smoothPoints[m][n][x][z] = (points[m][n][x][z + 1] + points[m][n][x + 1][z] + points[m][n][x][z - 1]) / 3;
					}
					else if (z == BigChunkSize && x != 0 && x != BigChunkSize) {
						smoothPoints[m][n][x][z] = (points[m][n][x - 1][z] + points[m][n][x + 1][z] + points[m][n][x][z - 1]) / 3;
					}
					else if (x == BigChunkSize && z != 0 && z != BigChunkSize) {
						smoothPoints[m][n][x][z] = (points[m][n][x][z + 1] + points[m][n][x - 1][z] + points[m][n][x][z - 1]) / 3;
					}
					else if (z == 0 && x != 0 && x != BigChunkSize) {
						smoothPoints[m][n][x][z] = (points[m][n][x][z + 1] + points[m][n][x + 1][z] + points[m][n][x - 1][z]) / 3;
					}
					smoothPointsAvg[m][n] += smoothPoints[m][n][x][z];
				}
			}

			smoothPointsAvg[m][n] = smoothPointsAvg[m][n] / ((BigChunkSize+1) * (BigChunkSize+1));
		}
	}

	for (int m = 0; m < 5; m++) {
		for (int n = 0; n < 5; n++) {
			for (int x = 0; x < BigChunkSize; x++) {
				for (int z = 0; z < BigChunkSize; z++) {
					allPoints[x + m * BigChunkSize][z + n * BigChunkSize] = smoothPoints[m][n][x][z];

					if (m == 2 && x == (BigChunkSize-1) && n == 2 && z == (BigChunkSize-1)) {
						allPoints[x + m * BigChunkSize + 1][z + n * BigChunkSize + 1] = smoothPoints[m][n][x + 1][z + 1];
					}
					else if (m == 2 && x == (BigChunkSize-1)) {
						allPoints[x + m * BigChunkSize + 1][z + n * BigChunkSize] = smoothPoints[m][n][x + 1][z];
					}
					else if (n == 2 && z == (BigChunkSize-1)) {
						allPoints[x + m * BigChunkSize][z + n * BigChunkSize + 1] = smoothPoints[m][n][x][z + 1];
					}
				}
			}
		}
	}

	//////////Terrain//////////
	for (int z = 0; z < BigChunkSize*5; z++) {
		for (int x = 0; x < BigChunkSize*5; x++) {
			for (int i = 0; i < 2; i++) {
				if (i == 0) {
					V.x = 1.0f;
					V.y = allPoints[x + 1][z + 1] - allPoints[x][z];
					V.z = 1.0f;
					W.x = 1.0f;
					W.y = allPoints[x + 1][z] - allPoints[x][z];
					W.z = 0.0f;
				}
				if (i == 1) {
					V.x = 1.0f;
					V.y = allPoints[x + 1][z + 1] - allPoints[x][z];
					V.z = 1.0f;
					W.x = 0.0f;
					W.y = allPoints[x][z + 1] - allPoints[x][z];
					W.z = 1.0f;
				}
				f_normals[i][x][z].x = (V.y*W.z) - (V.z*W.y);
				f_normals[i][x][z].y = (V.z*W.x) - (V.x*W.z);
				f_normals[i][x][z].z = (V.x*W.y) - (V.y*W.x);
				f_normals[i][x][z] = normalize(f_normals[i][x][z]);
				if (f_normals[i][x][z].y < 0.0f) {
					f_normals[i][x][z].x *= -1;
					f_normals[i][x][z].y *= -1;
					f_normals[i][x][z].z *= -1;
				}
			}
		}
	}

	for (int z = 0; z < BigChunkSize * 5 + 1; z++) {
		for (int x = 0; x < BigChunkSize * 5 + 1; x++) {
			if (x != 0 && z != 0 && x != BigChunkSize*5 && z != BigChunkSize * 5) {
				normals[x][z].x = ((f_normals[0][x - 1][z - 1].x + f_normals[1][x - 1][z - 1].x) / 2 + f_normals[0][x - 1][z].x + (f_normals[0][x][z].x + f_normals[1][x][z].x) / 2 + f_normals[1][x][z - 1].x) / 4;
				normals[x][z].y = ((f_normals[0][x - 1][z - 1].y + f_normals[1][x - 1][z - 1].y) / 2 + f_normals[0][x - 1][z].y + (f_normals[0][x][z].y + f_normals[1][x][z].y) / 2 + f_normals[1][x][z - 1].y) / 4;
				normals[x][z].z = ((f_normals[0][x - 1][z - 1].z + f_normals[1][x - 1][z - 1].z) / 2 + f_normals[0][x - 1][z].z + (f_normals[0][x][z].z + f_normals[1][x][z].z) / 2 + f_normals[1][x][z - 1].z) / 4;
				normals[x][z] = normalize(normals[x][z]);
			}
			else {////////////CHANGE//////////////
				normals[x][z].x = 0.0f;
				normals[x][z].y = -1.0f;
				normals[x][z].z = 0.0f;
			}
		}
	}
}


void createTerrain() {
	regionPos.x = 0;
	regionPos.z = 0;
	region.x = 0;
	region.z = 0;
	smallRegionPos.x = 0;
	smallRegionPos.z = 0;
	smallRegion.x = 0;
	smallRegion.z = 0;

	srand(seed);

	for (int x = 0; x < 1001; x++) {
		for (int z = 0; z < 1001; z++) {
			randSeed[x][z] = rand() % 1000000;
		}
	}
	//randSeed[500][500] = 1; ////Change eventually////


	counter = 0;
	calculateTerrain1(); // Even numbered chunks
	calculateTerrain2(); // Odd numbered chunks

	counter = 0;
	for (int m = 0; m < 7; m++) {
		for (int n = 0; n < 7; n++) {
			for (int z = 0; z < 33; z++) {
				for (int x = 0; x < 33; x++) {
					//if (allPoints[x+(smallRegion.x+m-3)*32-region.x*BigChunkSize+BigChunkSize*2][z+(smallRegion.z+n-3)*32-region.z*BigChunkSize+BigChunkSize*2] < 0.0f)
					//		allPoints[x+(smallRegion.x+m-3)*32-region.x*BigChunkSize+BigChunkSize*2][z+(smallRegion.z+n-3)*32-region.z*BigChunkSize+BigChunkSize*2] = 0.01f;
					terrainVertices_highQ[m][n][counter].pos.x = x + (smallRegion.x + m - 3) * 32;
					terrainVertices_highQ[m][n][counter].pos.y = allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2];
					terrainVertices_highQ[m][n][counter].pos.z = z + (smallRegion.z + n - 3) * 32;
					terrainVertices_highQ[m][n][counter].texCoord.x = float(x);
					terrainVertices_highQ[m][n][counter].texCoord.y = float(z);
					terrainVertices_highQ[m][n][counter].normal.x = normals[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2].x;
					terrainVertices_highQ[m][n][counter].normal.y = normals[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2].y;
					terrainVertices_highQ[m][n][counter].normal.z = normals[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2].z;

					if (x == 0 && m == 0 && float(z) / 2.0f != floor(float(z) / 2.0f)) {
						terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z - 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
					}
					if (x == 32 && m == 6 && float(z) / 2.0f != floor(float(z) / 2.0f)) {
						terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z - 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
					}
					if (z == 0 && n == 0 && float(x) / 2.0f != floor(float(x) / 2.0f)) {
						terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x - 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
					}
					if (z == 32 && n == 6 && float(x) / 2.0f != floor(float(x) / 2.0f)) {
						terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x - 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
					}
					counter++;
				}
			}
			counter = 0;
		}
	}

	counter = 0;

	for (int z = 0; z < 32; z++) {
		for (int x = 0; x < 32; x++) {
			terrainIndices_highQ[counter] = x + z * 33;
			counter++;
			terrainIndices_highQ[counter] = x + 1 + z * 33;
			counter++;
			terrainIndices_highQ[counter] = x + 34 + z * 33;
			counter++;
			terrainIndices_highQ[counter] = x + z * 33;
			counter++;
			terrainIndices_highQ[counter] = x + 34 + z * 33;
			counter++;
			terrainIndices_highQ[counter] = x + 33 + z * 33;
			counter++;
		}
	}


	ZeroMemory(&terrainIndexBufferDesc_highQ, sizeof(terrainIndexBufferDesc_highQ));

	terrainIndexBufferDesc_highQ.Usage = D3D11_USAGE_DEFAULT;
	terrainIndexBufferDesc_highQ.BindFlags = D3D11_BIND_INDEX_BUFFER;
	terrainIndexBufferDesc_highQ.CPUAccessFlags = 0;
	terrainIndexBufferDesc_highQ.MiscFlags = 0;
	terrainIndexBufferDesc_highQ.ByteWidth = sizeof(terrainIndices_highQ);

	terrainIndexBufferData_highQ.pSysMem = terrainIndices_highQ;
	d3d11Device->CreateBuffer(&terrainIndexBufferDesc_highQ, &terrainIndexBufferData_highQ, &terrainIndexBuffer_highQ);

	ZeroMemory(&terrainVertexBufferData_highQ, sizeof(terrainVertexBufferData_highQ));
	ZeroMemory(&terrainVertexBufferDesc_highQ, sizeof(terrainVertexBufferDesc_highQ));

	terrainVertexBufferDesc_highQ.Usage = D3D11_USAGE_DEFAULT;
	terrainVertexBufferDesc_highQ.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	terrainVertexBufferDesc_highQ.CPUAccessFlags = 0;
	terrainVertexBufferDesc_highQ.MiscFlags = 0;
	terrainVertexBufferDesc_highQ.ByteWidth = sizeof(terrainVertices_highQ[0][0]);

	for (int m = 0; m < 7; m++) {
		for (int n = 0; n < 7; n++) {
			terrainVertexBufferData_highQ[m][n].pSysMem = terrainVertices_highQ[m][n];
			hr = d3d11Device->CreateBuffer(&terrainVertexBufferDesc_highQ, &terrainVertexBufferData_highQ[m][n], &terrainVertBuffer_highQ[m][n]);
		}
	}
}

//bool drawTerrain[69];

void updateTerrain(double3 camPos) {
	regionPos.x = int(floor(camPos.x / BigChunkSize));
	regionPos.z = int(floor(camPos.z / BigChunkSize));
	smallRegionPos.x = int(floor(camPos.x / 32));
	smallRegionPos.z = int(floor(camPos.z / 32));


	if (region.x != regionPos.x || region.z != regionPos.z) {
		region.x = regionPos.x;
		region.z = regionPos.z;
		change = true;
		smallChange = true;
	}

	if (smallRegion.x != smallRegionPos.x || smallRegion.z != smallRegionPos.z) {
		smallRegion.x = smallRegionPos.x;
		smallRegion.z = smallRegionPos.z;
		smallChange = true;
	}


	if (change == true) {
		calculateTerrain1();
		calculateTerrain2();

		change = false;
	}

	if (smallChange == true && change == false) {
		counter = 0;

		for (int m = 0; m < 7; m++) {
			for (int n = 0; n < 7; n++) {
				for (int z = 0; z < 33; z++) {
					for (int x = 0; x < 33; x++) {
						//if (allPoints[x+(smallRegion.x+m-3)*32-region.x*BigChunkSize+BigChunkSize*2][z+(smallRegion.z+n-3)*32-region.z*BigChunkSize+BigChunkSize*2] < 0.0f)
						//	allPoints[x+(smallRegion.x+m-3)*32-region.x*BigChunkSize+BigChunkSize*2][z+(smallRegion.z+n-3)*32-region.z*BigChunkSize+BigChunkSize*2] = 0.01f;
						terrainVertices_highQ[m][n][counter].pos.x = x + (smallRegion.x + m - 3) * 32 -region.x*BigChunkSize;
						terrainVertices_highQ[m][n][counter].pos.y = allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2];
						terrainVertices_highQ[m][n][counter].pos.z = z + (smallRegion.z + n - 3) * 32 -region.z*BigChunkSize;
						terrainVertices_highQ[m][n][counter].texCoord.x = float(x);
						terrainVertices_highQ[m][n][counter].texCoord.y = float(z);
						terrainVertices_highQ[m][n][counter].normal.x = normals[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2].x;
						terrainVertices_highQ[m][n][counter].normal.y = normals[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2].y;
						terrainVertices_highQ[m][n][counter].normal.z = normals[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2].z;

						if (x == 0 && m == 0 && float(z) / 2.0f != floor(float(z) / 2.0f)) {
							terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z - 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
						}
						if (x == 32 && m == 6 && float(z) / 2.0f != floor(float(z) / 2.0f)) {
							terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z - 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + 1 + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
						}
						if (z == 0 && n == 0 && float(x) / 2.0f != floor(float(x) / 2.0f)) {
							terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x - 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
						}
						if (z == 32 && n == 6 && float(x) / 2.0f != floor(float(x) / 2.0f)) {
							terrainVertices_highQ[m][n][counter].pos.y = (allPoints[x - 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2] + allPoints[x + 1 + (smallRegion.x + m - 3) * 32 - region.x * BigChunkSize + BigChunkSize*2][z + (smallRegion.z + n - 3) * 32 - region.z * BigChunkSize + BigChunkSize*2]) / 2.0f;
						}
						counter++;
					}
				}
				counter = 0;
			}
		}

		for (int m = 0; m < 7; m++) {
			for (int n = 0; n < 7; n++) {
				terrainVertexBufferData_highQ[m][n].pSysMem = terrainVertices_highQ[m][n];
				hr = d3d11Device->CreateBuffer(&terrainVertexBufferDesc_highQ, &terrainVertexBufferData_highQ[m][n], &terrainVertBuffer_highQ[m][n]);
			}
		}


		smallChange = false;
	}

	//Reset groundWorld
	groundWorld_highQ = XMMatrixIdentity();

	//Define ground's world space matrix
	Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	//Translation = XMMatrixTranslation(-camPos.x, -camPos.y, -camPos.z);
	Translation = XMMatrixTranslation(region.x*BigChunkSize - camPos.x, -camPos.y, region.z*BigChunkSize - camPos.z);

	//Set ground's world space using the transformations
	groundWorld_highQ = Scale * Translation;
}

void drawTerrain() {
	//Set Vertex and Pixel Shaders
	d3d11DevCon->VSSetShader(VS, 0, 0);
	d3d11DevCon->PSSetShader(PS, 0, 0);

	//Set the Input Layout
	d3d11DevCon->IASetInputLayout(vertLayout);

	UINT stride1 = sizeof(Vertex);
	UINT offset1 = 0;
	//High Quality
	counter = 0;
	for (int m = 0; m < 7; m++) {
		for (int n = 0; n < 7; n++) {
			//if (drawTerrain[counter] == true || m == 2 && n == 2){
			d3d11DevCon->IASetIndexBuffer(terrainIndexBuffer_highQ, DXGI_FORMAT_R32_UINT, 0);
			d3d11DevCon->IASetVertexBuffers(0, 1, &terrainVertBuffer_highQ[m][n], &stride1, &offset1);

			//Set the WVP matrix and send it to the constant buffer in effect file
			WVP = groundWorld_highQ * camView * camProjection;
			cbPerObj.WVP = XMMatrixTranspose(WVP);
			cbPerObj.World = XMMatrixTranspose(groundWorld_highQ);
			d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
			d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
			d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
			d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

			d3d11DevCon->RSSetState(CCWcullMode);
			d3d11DevCon->DrawIndexed(6 * 32 * 32, 0, 0);
			//}
			counter++;
		}
	}
}

void cleanUpTerrain() {
	for (int m = 0; m < 7; m++) {
		for (int n = 0; n < 7; n++) {
			terrainVertBuffer_highQ[m][n]->Release();
		}
	}
	terrainIndexBuffer_highQ->Release();
}

#endif