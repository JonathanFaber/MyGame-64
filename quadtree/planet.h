#include "includes.h"

#ifndef __PLANET_H_INCLUDED__
#define __PLANET_H_INCLUDED__

double3 cubePos;
double planetRadius = 5340353.71544;



class planet {
private:
	static const int LandChunkSize = 256;

	ID3D11Buffer* indexBuffer[6];
	ID3D11Buffer* vertBuffer[6];

	XMMATRIX groundWorld;

	PlanetVertex vertices[6][(LandChunkSize + 1)*(LandChunkSize + 1)];
	DWORD indices[6][LandChunkSize*LandChunkSize * 6];

	float landPoints[LandChunkSize + 1][LandChunkSize + 1];
	float landPoints_x[2][LandChunkSize + 1][LandChunkSize + 1];
	float landPoints_y[LandChunkSize + 1][2][LandChunkSize + 1];
	float landPoints_z[LandChunkSize + 1][LandChunkSize + 1][2];

	float landPoints_med[1][1][LandChunkSize + 1][LandChunkSize + 1];
	//
	float heatPoints[LandChunkSize + 1][LandChunkSize + 1];
	float heatPoints_x[2][LandChunkSize + 1][LandChunkSize + 1];
	float heatPoints_y[LandChunkSize + 1][2][LandChunkSize + 1];
	float heatPoints_z[LandChunkSize + 1][LandChunkSize + 1][2];
	//
	float moisturePoints[LandChunkSize + 1][LandChunkSize + 1];
	float moisturePoints_x[2][LandChunkSize + 1][LandChunkSize + 1];
	float moisturePoints_y[LandChunkSize + 1][2][LandChunkSize + 1];
	float moisturePoints_z[LandChunkSize + 1][LandChunkSize + 1][2];

	bool landDone[LandChunkSize + 1][LandChunkSize + 1];
	int landLength;
	float amplitude;
	float randomH;

	int seed;
	int randSeed_large[18];

	bool drawFace[8];
	int closestFace;

	UINT omitIndices[6][25]; // second [] is how many squares
	UINT omitIndicesOrdered[6][25];
	int numSquaresToOmit[6];

	double3 planetPos;
	double3 camToPlanet;
	double3 normalizedCamToPlanet;

	// medium
	ID3D11Buffer* terrainIndexBuffer_highQ;
	ID3D11Buffer* terrainVertBuffer_highQ[7][7];
	vec2 terrain_mediumQ_Pos[44];
	XMMATRIX groundWorld_highQ;

	static const int BigChunkSize = 256;

	float points[5][5][BigChunkSize + 1][BigChunkSize + 1];
	float smoothPoints[5][5][BigChunkSize + 1][BigChunkSize + 1];
	float allPoints[BigChunkSize * 5 + 1][BigChunkSize * 5 + 1];
	float smoothPointsAvg[5][5];

	static const int random = 5;

	bool done[BigChunkSize + 1][BigChunkSize + 1];
	int length = BigChunkSize;

	bool BigChunkDone[5][5];

	vec2 regionPos;
	vec2 region;
	vec2 smallRegionPos;
	vec2 smallRegion;
	vec2 bigRegionPos;
	vec2 bigRegion;

	bool change = false;
	bool smallChange = false;
	int randSeed_medium[10001][10001];

	Vertex terrainVertices_highQ[7][7][33 * 33];
	DWORD terrainIndices_highQ[6 * 32 * 32];

	XMFLOAT3 f_normals[2][BigChunkSize * 5][BigChunkSize * 5];
	XMFLOAT3 V;
	XMFLOAT3 W;
	XMFLOAT3 normals[BigChunkSize * 5 + 1][BigChunkSize * 5 + 1];

	D3D11_BUFFER_DESC terrainIndexBufferDesc_highQ;
	D3D11_SUBRESOURCE_DATA terrainIndexBufferData_highQ;
	D3D11_BUFFER_DESC terrainVertexBufferDesc_highQ;
	D3D11_SUBRESOURCE_DATA terrainVertexBufferData_highQ[7][7];

public:
	XMFLOAT3 planetPoints_x[2][LandChunkSize + 1][LandChunkSize + 1];
	XMFLOAT3 planetPoints_y[LandChunkSize + 1][2][LandChunkSize + 1];
	XMFLOAT3 planetPoints_z[LandChunkSize + 1][LandChunkSize + 1][2];

	//points have to be between -1 and 1
	//not 0 and 1

	void createLand() {
		landLength = LandChunkSize;

		seed = 1;

		counter = 0;
		srand(seed);

		for (int i = 0; i < 18; i++) {
			randSeed_large[i] = rand() % 32000;
		}
		//randSeed_large [500][500] = 1;////Change eventually////

		for (int m = 0; m < 2; m++) {
			for (int z = 0; z < LandChunkSize + 1; z++) {
				for (int x = 0; x < LandChunkSize + 1; x++) {
					landPoints_x[m][x][z] = 0.0f;
					landPoints_y[x][m][z] = 0.0f;
					landPoints_z[x][z][m] = 0.0f;
					heatPoints_x[m][x][z] = 0.0f;
					heatPoints_y[x][m][z] = 0.0f;
					heatPoints_z[x][z][m] = 0.0f;
					moisturePoints_x[m][x][z] = 0.0f;
					moisturePoints_y[x][m][z] = 0.0f;
					moisturePoints_z[x][z][m] = 0.0f;
				}
			}
		}

		counter = 0;
		for (int m = 0; m < 6; m++) {
			for (int z = 0; z < LandChunkSize + 1; z++) {
				for (int x = 0; x < LandChunkSize + 1; x++) {
					landPoints[x][z] = 0.0f;
					landDone[x][z] = false;
				}
			}

			amplitude = 1.0f;
			srand(randSeed_large[m]);

			if (m == 2) { //x side 0
				for (int i = 0; i < LandChunkSize + 1; i++) {
					landPoints[i][0] = landPoints_y[0][0][i];
					landDone[i][0] = true;

					landPoints[i][LandChunkSize] = landPoints_y[0][1][i];
					landDone[i][LandChunkSize] = true;
				}
			}
			if (m == 3) { //x side 1
				for (int i = 0; i < LandChunkSize + 1; i++) {
					landPoints[i][0] = landPoints_y[LandChunkSize][0][i];
					landDone[i][0] = true;

					landPoints[i][LandChunkSize] = landPoints_y[LandChunkSize][1][i];
					landDone[i][LandChunkSize] = true;
				}
			}
			if (m == 4) { //z side 0
				for (int i = 0; i < LandChunkSize + 1; i++) {
					landPoints[i][0] = landPoints_y[i][0][0];
					landDone[i][0] = true;

					landPoints[i][LandChunkSize] = landPoints_y[i][1][0];
					landDone[i][LandChunkSize] = true;

					landPoints[0][i] = landPoints_x[0][i][0];
					landDone[0][i] = true;

					landPoints[LandChunkSize][i] = landPoints_x[1][i][0];
					landDone[LandChunkSize][i] = true;
				}
			}
			if (m == 5) { //z side 1
				for (int i = 0; i < LandChunkSize + 1; i++) {
					landPoints[i][0] = landPoints_y[i][0][LandChunkSize];
					landDone[i][0] = true;

					landPoints[i][LandChunkSize] = landPoints_y[i][1][LandChunkSize];
					landDone[i][LandChunkSize] = true;

					landPoints[0][i] = landPoints_x[0][i][LandChunkSize];
					landDone[0][i] = true;

					landPoints[LandChunkSize][i] = landPoints_x[1][i][LandChunkSize];
					landDone[LandChunkSize][i] = true;
				}
			}

			for (int i = 0; i < 8; i++) {
				for (int x = 0; x < LandChunkSize / landLength; x++) {
					for (int z = 0; z < LandChunkSize / landLength; z++) {
						//Middle
						if (landDone[landLength / 2 + x*landLength][landLength / 2 + z*landLength] == false) {
							randomH = (rand() % 1000) - 500;
							landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] = (landPoints[x*landLength][z*landLength] + landPoints[x*landLength][(z + 1)*landLength] + landPoints[(x + 1)*landLength][(z + 1)*landLength] + landPoints[(x + 1)*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
						}
					}
				}

				for (int x = 0; x < LandChunkSize / landLength; x++) {
					for (int z = 0; z < LandChunkSize / landLength; z++) {
						if (landDone[x*landLength][landLength / 2 + z*landLength] == false) {	//west
							randomH = (rand() % int(1000)) - 500;
							if (x != 0) {
								landPoints[x*landLength][landLength / 2 + z*landLength] = (landPoints[x*landLength - landLength / 2][landLength / 2 + z*landLength] + landPoints[x*landLength][landLength + z*landLength] + landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + landPoints[x*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//landPoints[x*landLength][landLength/2+z*landLength] = (landPoints[x*landLength][landLength+z*landLength] + landPoints[landLength/2+x*landLength][landLength/2+z*landLength] + landPoints[x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								landPoints[x*landLength][landLength / 2 + z*landLength] = (landPoints[x*landLength][landLength + z*landLength] + landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + landPoints[x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[x*landLength][landLength / 2 + z*landLength] = true;
						}
						if (landDone[landLength / 2 + x*landLength][landLength + z*landLength] == false) {	//north
							randomH = (rand() % int(1000)) - 500;
							if (z != LandChunkSize / landLength - 1) {
								landPoints[landLength / 2 + x*landLength][landLength + z*landLength] = (landPoints[x*landLength][landLength + z*landLength] + landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength + landLength] + landPoints[landLength + x*landLength][landLength + z*landLength] + landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//landPoints[landLength/2+x*landLength][landLength+z*landLength] = (landPoints[x*landLength][landLength+z*landLength] + landPoints[landLength+x*landLength][landLength+z*landLength] + landPoints[landLength/2+x*landLength][landLength/2+z*landLength])/3 + (randomH/500.0f)*amplitude;
								landPoints[landLength / 2 + x*landLength][landLength + z*landLength] = (landPoints[x*landLength][landLength + z*landLength] + landPoints[landLength + x*landLength][landLength + z*landLength] + landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength / 2 + x*landLength][landLength + z*landLength] = true;
						}
						if (landDone[landLength + x*landLength][landLength / 2 + z*landLength] == false) {	//east
							randomH = (rand() % int(1000)) - 500;
							if (x != LandChunkSize / landLength - 1) {
								landPoints[landLength + x*landLength][landLength / 2 + z*landLength] = (landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + landPoints[landLength + x*landLength][landLength + z*landLength] + landPoints[landLength + x*landLength + landLength / 2][landLength / 2 + z*landLength] + landPoints[landLength + x*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//landPoints[landLength+x*landLength][landLength/2+z*landLength] = (landPoints[landLength/2+x*landLength][landLength/2+z*landLength] + landPoints[landLength+x*landLength][landLength+z*landLength] + landPoints[landLength+x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								landPoints[landLength + x*landLength][landLength / 2 + z*landLength] = (landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + landPoints[landLength + x*landLength][landLength + z*landLength] + landPoints[landLength + x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength + x*landLength][landLength / 2 + z*landLength] = true;
						}
						if (landDone[landLength / 2 + x*landLength][z*landLength] == false) {	//south
							randomH = (rand() % int(1000)) - 500;
							if (z != 0) {
								landPoints[landLength / 2 + x*landLength][z*landLength] = (landPoints[x*landLength][z*landLength] + landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + landPoints[landLength + x*landLength][z*landLength] + landPoints[landLength / 2 + x*landLength][z*landLength - landLength / 2]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//landPoints[landLength/2+x*landLength][z*landLength] = (landPoints[x*landLength][z*landLength] + landPoints[landLength/2+x*landLength][landLength/2+z*landLength] + landPoints[landLength+x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								landPoints[landLength / 2 + x*landLength][z*landLength] = (landPoints[x*landLength][z*landLength] + landPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + landPoints[landLength + x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength / 2 + x*landLength][z*landLength] = true;
						}
					}
				}

				amplitude *= 0.5f;
				landLength *= 0.5;
			}
			///////////////////////////
			landLength = LandChunkSize;
			amplitude = 1.0f;

			for (int z = 0; z < LandChunkSize + 1; z++) {
				for (int x = 0; x < LandChunkSize + 1; x++) {
					if (m == 0 || m == 1) {
						landPoints_y[x][m][z] = landPoints[x][z];
					}
					if (m == 2 || m == 3) {
						landPoints_x[m - 2][z][x] = landPoints[x][z];
					}
					if (m == 4 || m == 5) {
						landPoints_z[x][z][m - 4] = landPoints[x][z];
					}
				}
			}

		}


	}

	void createTemperature() {
		landLength = LandChunkSize;

		counter = 0;
		for (int m = 0; m < 6; m++) {
			for (int z = 0; z < LandChunkSize + 1; z++) {
				for (int x = 0; x < LandChunkSize + 1; x++) {
					heatPoints[x][z] = 0.0f;
					landDone[x][z] = false;
				}
			}

			amplitude = 1.0f;
			srand(randSeed_large[m + 6]);

			if (m == 0) {
				heatPoints[LandChunkSize / 2][LandChunkSize / 2] = -1.0f;
				landDone[LandChunkSize / 2][LandChunkSize / 2] = true;
			}
			else if (m == 1) {
				heatPoints[LandChunkSize / 2][LandChunkSize / 2] = -1.0f;
				landDone[LandChunkSize / 2][LandChunkSize / 2] = true;
			}
			else if (m == 2) { //x side 0
				heatPoints[LandChunkSize / 2][LandChunkSize / 2] = 1.0f;
				landDone[LandChunkSize / 2][LandChunkSize / 2] = true;

				for (int i = 0; i < LandChunkSize + 1; i++) {
					heatPoints[i][0] = heatPoints_y[0][0][i];
					landDone[i][0] = true;

					heatPoints[i][LandChunkSize] = heatPoints_y[0][1][i];
					landDone[i][LandChunkSize] = true;
				}
			}
			else if (m == 3) { //x side 1
				heatPoints[LandChunkSize / 2][LandChunkSize / 2] = 1.0f;
				landDone[LandChunkSize / 2][LandChunkSize / 2] = true;

				for (int i = 0; i < LandChunkSize + 1; i++) {
					heatPoints[i][0] = heatPoints_y[LandChunkSize][0][i];
					landDone[i][0] = true;

					heatPoints[i][LandChunkSize] = heatPoints_y[LandChunkSize][1][i];
					landDone[i][LandChunkSize] = true;
				}
			}
			else if (m == 4) { //z side 0
				heatPoints[LandChunkSize / 2][LandChunkSize / 2] = 1.0f;
				landDone[LandChunkSize / 2][LandChunkSize / 2] = true;

				for (int i = 0; i < LandChunkSize + 1; i++) {
					heatPoints[i][0] = heatPoints_y[i][0][0];
					landDone[i][0] = true;

					heatPoints[i][LandChunkSize] = heatPoints_y[i][1][0];
					landDone[i][LandChunkSize] = true;

					heatPoints[0][i] = heatPoints_x[0][i][0];
					landDone[0][i] = true;

					heatPoints[LandChunkSize][i] = heatPoints_x[1][i][0];
					landDone[LandChunkSize][i] = true;
				}
			}
			else if (m == 5) { //z side 1
				heatPoints[LandChunkSize / 2][LandChunkSize / 2] = 1.0f;
				landDone[LandChunkSize / 2][LandChunkSize / 2] = true;

				for (int i = 0; i < LandChunkSize + 1; i++) {
					heatPoints[i][0] = heatPoints_y[i][0][LandChunkSize];
					landDone[i][0] = true;

					heatPoints[i][LandChunkSize] = heatPoints_y[i][1][LandChunkSize];
					landDone[i][LandChunkSize] = true;

					heatPoints[0][i] = heatPoints_x[0][i][LandChunkSize];
					landDone[0][i] = true;

					heatPoints[LandChunkSize][i] = heatPoints_x[1][i][LandChunkSize];
					landDone[LandChunkSize][i] = true;
				}
			}

			for (int i = 0; i < 8; i++) {
				for (int x = 0; x < LandChunkSize / landLength; x++) {
					for (int z = 0; z < LandChunkSize / landLength; z++) {
						//Middle
						if (landDone[landLength / 2 + x*landLength][landLength / 2 + z*landLength] == false) {
							randomH = (rand() % 1000) - 500;
							heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] = (heatPoints[x*landLength][z*landLength] + heatPoints[x*landLength][(z + 1)*landLength] + heatPoints[(x + 1)*landLength][(z + 1)*landLength] + heatPoints[(x + 1)*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
						}
					}
				}

				for (int x = 0; x < LandChunkSize / landLength; x++) {
					for (int z = 0; z < LandChunkSize / landLength; z++) {
						if (landDone[x*landLength][landLength / 2 + z*landLength] == false) {	//west
							randomH = (rand() % int(1000)) - 500;
							if (x != 0) {
								heatPoints[x*landLength][landLength / 2 + z*landLength] = (heatPoints[x*landLength - landLength / 2][landLength / 2 + z*landLength] + heatPoints[x*landLength][landLength + z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + heatPoints[x*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//heatPoints[x*landLength][landLength/2+z*landLength] = (heatPoints[x*landLength][landLength+z*landLength] + heatPoints[landLength/2+x*landLength][landLength/2+z*landLength] + heatPoints[x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								heatPoints[x*landLength][landLength / 2 + z*landLength] = (heatPoints[x*landLength][landLength + z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + heatPoints[x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[x*landLength][landLength / 2 + z*landLength] = true;
						}
						if (landDone[landLength / 2 + x*landLength][landLength + z*landLength] == false) {	//north
							randomH = (rand() % int(1000)) - 500;
							if (z != LandChunkSize / landLength - 1) {
								heatPoints[landLength / 2 + x*landLength][landLength + z*landLength] = (heatPoints[x*landLength][landLength + z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength + landLength] + heatPoints[landLength + x*landLength][landLength + z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//heatPoints[landLength/2+x*landLength][landLength+z*landLength] = (heatPoints[x*landLength][landLength+z*landLength] + heatPoints[landLength+x*landLength][landLength+z*landLength] + heatPoints[landLength/2+x*landLength][landLength/2+z*landLength])/3 + (randomH/500.0f)*amplitude;
								heatPoints[landLength / 2 + x*landLength][landLength + z*landLength] = (heatPoints[x*landLength][landLength + z*landLength] + heatPoints[landLength + x*landLength][landLength + z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength / 2 + x*landLength][landLength + z*landLength] = true;
						}
						if (landDone[landLength + x*landLength][landLength / 2 + z*landLength] == false) {	//east
							randomH = (rand() % int(1000)) - 500;
							if (x != LandChunkSize / landLength - 1) {
								heatPoints[landLength + x*landLength][landLength / 2 + z*landLength] = (heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + heatPoints[landLength + x*landLength][landLength + z*landLength] + heatPoints[landLength + x*landLength + landLength / 2][landLength / 2 + z*landLength] + heatPoints[landLength + x*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//heatPoints[landLength+x*landLength][landLength/2+z*landLength] = (heatPoints[landLength/2+x*landLength][landLength/2+z*landLength] + heatPoints[landLength+x*landLength][landLength+z*landLength] + heatPoints[landLength+x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								heatPoints[landLength + x*landLength][landLength / 2 + z*landLength] = (heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + heatPoints[landLength + x*landLength][landLength + z*landLength] + heatPoints[landLength + x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength + x*landLength][landLength / 2 + z*landLength] = true;
						}
						if (landDone[landLength / 2 + x*landLength][z*landLength] == false) {	//south
							randomH = (rand() % int(1000)) - 500;
							if (z != 0) {
								heatPoints[landLength / 2 + x*landLength][z*landLength] = (heatPoints[x*landLength][z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + heatPoints[landLength + x*landLength][z*landLength] + heatPoints[landLength / 2 + x*landLength][z*landLength - landLength / 2]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								heatPoints[landLength / 2 + x*landLength][z*landLength] = (heatPoints[x*landLength][z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + heatPoints[landLength + x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
								heatPoints[landLength / 2 + x*landLength][z*landLength] = (heatPoints[x*landLength][z*landLength] + heatPoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + heatPoints[landLength + x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength / 2 + x*landLength][z*landLength] = true;
						}
					}
				}

				amplitude *= 0.5f;
				landLength *= 0.5;
			}
			///////////////////////////
			landLength = LandChunkSize;
			amplitude = 1.0f;


			for (int z = 0; z < LandChunkSize + 1; z++) {
				for (int x = 0; x < LandChunkSize + 1; x++) {
					if (m == 0 || m == 1) {
						heatPoints_y[x][m][z] = heatPoints[x][z];
					}
					if (m == 2 || m == 3) {
						heatPoints_x[m - 2][z][x] = heatPoints[x][z];
					}
					if (m == 4 || m == 5) {
						heatPoints_z[x][z][m - 4] = heatPoints[x][z];
					}
				}
			}

		}


	}

	void createMoisture() {
		landLength = LandChunkSize;

		counter = 0;
		for (int m = 0; m < 6; m++) {
			for (int z = 0; z < LandChunkSize + 1; z++) {
				for (int x = 0; x < LandChunkSize + 1; x++) {
					moisturePoints[x][z] = 0.0f;
					landDone[x][z] = false;
				}
			}

			amplitude = 1.0f;
			srand(randSeed_large[m + 12]);

			if (m == 2) { //x side 0
				for (int i = 0; i < LandChunkSize + 1; i++) {
					moisturePoints[i][0] = moisturePoints_y[0][0][i];
					landDone[i][0] = true;

					moisturePoints[i][LandChunkSize] = moisturePoints_y[0][1][i];
					landDone[i][LandChunkSize] = true;
				}
			}
			else if (m == 3) { //x side 1
				for (int i = 0; i < LandChunkSize + 1; i++) {
					moisturePoints[i][0] = moisturePoints_y[LandChunkSize][0][i];
					landDone[i][0] = true;

					moisturePoints[i][LandChunkSize] = moisturePoints_y[LandChunkSize][1][i];
					landDone[i][LandChunkSize] = true;
				}
			}
			else if (m == 4) { //z side 0
				for (int i = 0; i < LandChunkSize + 1; i++) {
					moisturePoints[i][0] = moisturePoints_y[i][0][0];
					landDone[i][0] = true;

					moisturePoints[i][LandChunkSize] = moisturePoints_y[i][1][0];
					landDone[i][LandChunkSize] = true;

					moisturePoints[0][i] = moisturePoints_x[0][i][0];
					landDone[0][i] = true;

					moisturePoints[LandChunkSize][i] = moisturePoints_x[1][i][0];
					landDone[LandChunkSize][i] = true;
				}
			}
			else if (m == 5) { //z side 1
				for (int i = 0; i < LandChunkSize + 1; i++) {
					moisturePoints[i][0] = moisturePoints_y[i][0][LandChunkSize];
					landDone[i][0] = true;

					moisturePoints[i][LandChunkSize] = moisturePoints_y[i][1][LandChunkSize];
					landDone[i][LandChunkSize] = true;

					moisturePoints[0][i] = moisturePoints_x[0][i][LandChunkSize];
					landDone[0][i] = true;

					moisturePoints[LandChunkSize][i] = moisturePoints_x[1][i][LandChunkSize];
					landDone[LandChunkSize][i] = true;
				}
			}

			for (int i = 0; i < 8; i++) {
				for (int x = 0; x < LandChunkSize / landLength; x++) {
					for (int z = 0; z < LandChunkSize / landLength; z++) {
						//Middle
						if (landDone[landLength / 2 + x*landLength][landLength / 2 + z*landLength] == false) {
							randomH = (rand() % 1000) - 500;
							moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] = (moisturePoints[x*landLength][z*landLength] + moisturePoints[x*landLength][(z + 1)*landLength] + moisturePoints[(x + 1)*landLength][(z + 1)*landLength] + moisturePoints[(x + 1)*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
						}
					}
				}

				for (int x = 0; x < LandChunkSize / landLength; x++) {
					for (int z = 0; z < LandChunkSize / landLength; z++) {
						if (landDone[x*landLength][landLength / 2 + z*landLength] == false) {	//west
							randomH = (rand() % int(1000)) - 500;
							if (x != 0) {
								moisturePoints[x*landLength][landLength / 2 + z*landLength] = (moisturePoints[x*landLength - landLength / 2][landLength / 2 + z*landLength] + moisturePoints[x*landLength][landLength + z*landLength] + moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + moisturePoints[x*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//moisturePoints[x*landLength][landLength/2+z*landLength] = (moisturePoints[x*landLength][landLength+z*landLength] + moisturePoints[landLength/2+x*landLength][landLength/2+z*landLength] + moisturePoints[x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								moisturePoints[x*landLength][landLength / 2 + z*landLength] = (moisturePoints[x*landLength][landLength + z*landLength] + moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + moisturePoints[x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[x*landLength][landLength / 2 + z*landLength] = true;
						}
						if (landDone[landLength / 2 + x*landLength][landLength + z*landLength] == false) {	//north
							randomH = (rand() % int(1000)) - 500;
							if (z != LandChunkSize / landLength - 1) {
								moisturePoints[landLength / 2 + x*landLength][landLength + z*landLength] = (moisturePoints[x*landLength][landLength + z*landLength] + moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength + landLength] + moisturePoints[landLength + x*landLength][landLength + z*landLength] + moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//moisturePoints[landLength/2+x*landLength][landLength+z*landLength] = (moisturePoints[x*landLength][landLength+z*landLength] + moisturePoints[landLength+x*landLength][landLength+z*landLength] + moisturePoints[landLength/2+x*landLength][landLength/2+z*landLength])/3 + (randomH/500.0f)*amplitude;
								moisturePoints[landLength / 2 + x*landLength][landLength + z*landLength] = (moisturePoints[x*landLength][landLength + z*landLength] + moisturePoints[landLength + x*landLength][landLength + z*landLength] + moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength / 2 + x*landLength][landLength + z*landLength] = true;
						}
						if (landDone[landLength + x*landLength][landLength / 2 + z*landLength] == false) {	//east
							randomH = (rand() % int(1000)) - 500;
							if (x != LandChunkSize / landLength - 1) {
								moisturePoints[landLength + x*landLength][landLength / 2 + z*landLength] = (moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + moisturePoints[landLength + x*landLength][landLength + z*landLength] + moisturePoints[landLength + x*landLength + landLength / 2][landLength / 2 + z*landLength] + moisturePoints[landLength + x*landLength][z*landLength]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//moisturePoints[landLength+x*landLength][landLength/2+z*landLength] = (moisturePoints[landLength/2+x*landLength][landLength/2+z*landLength] + moisturePoints[landLength+x*landLength][landLength+z*landLength] + moisturePoints[landLength+x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								moisturePoints[landLength + x*landLength][landLength / 2 + z*landLength] = (moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + moisturePoints[landLength + x*landLength][landLength + z*landLength] + moisturePoints[landLength + x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength + x*landLength][landLength / 2 + z*landLength] = true;
						}
						if (landDone[landLength / 2 + x*landLength][z*landLength] == false) {	//south
							randomH = (rand() % int(1000)) - 500;
							if (z != 0) {
								moisturePoints[landLength / 2 + x*landLength][z*landLength] = (moisturePoints[x*landLength][z*landLength] + moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + moisturePoints[landLength + x*landLength][z*landLength] + moisturePoints[landLength / 2 + x*landLength][z*landLength - landLength / 2]) / 4 + (randomH / 500.0f)*amplitude;
							}
							else {
								//moisturePoints[landLength/2+x*landLength][z*landLength] = (moisturePoints[x*landLength][z*landLength] + moisturePoints[landLength/2+x*landLength][landLength/2+z*landLength] + moisturePoints[landLength+x*landLength][z*landLength])/3 + (randomH/500.0f)*amplitude;
								moisturePoints[landLength / 2 + x*landLength][z*landLength] = (moisturePoints[x*landLength][z*landLength] + moisturePoints[landLength / 2 + x*landLength][landLength / 2 + z*landLength] + moisturePoints[landLength + x*landLength][z*landLength]) / 3 + (randomH / 500.0f)*amplitude;
							}
							landDone[landLength / 2 + x*landLength][z*landLength] = true;
						}
					}
				}

				amplitude *= 0.5f;
				landLength *= 0.5;
			}
			///////////////////////////
			landLength = LandChunkSize;
			amplitude = 1.0f;


			for (int z = 0; z < LandChunkSize + 1; z++) {
				for (int x = 0; x < LandChunkSize + 1; x++) {
					if (m == 0 || m == 1) {
						moisturePoints_y[x][m][z] = moisturePoints[x][z];
					}
					if (m == 2 || m == 3) {
						moisturePoints_x[m - 2][z][x] = moisturePoints[x][z];
					}
					if (m == 4 || m == 5) {
						moisturePoints_z[x][z][m - 4] = moisturePoints[x][z];
					}
				}
			}

		}


	}


	void create() {
		planetPos = double3(0.0, 0.0, 0.0);
		camToPlanet = double3(camPos.x - planetPos.x, camPos.y - planetPos.y, camPos.z - planetPos.z);
		normalizedCamToPlanet = normalize(camToPlanet);
		cubePos = normalizedCamToPlanet;
		cubizePoint(cubePos);


		// large
		createLand();
		createTemperature();
		createMoisture();


		for (int y = -(LandChunkSize / 2); y < (LandChunkSize / 2) + 1; y++) {
			for (int z = -(LandChunkSize / 2); z < (LandChunkSize / 2) + 1; z++) {
				int x = -1;
				planetPoints_x[0][y + (LandChunkSize / 2)][z + (LandChunkSize / 2)].x = float(x) * sqrt(1.0f - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) + ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 3));
				planetPoints_x[0][y + (LandChunkSize / 2)][z + (LandChunkSize / 2)].y = float(y) / (LandChunkSize / 2) * sqrt(1.0f - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) - (float(x)*float(x) / 2) + ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)*float(x)*float(x)) / 3));
				planetPoints_x[0][y + (LandChunkSize / 2)][z + (LandChunkSize / 2)].z = float(z) / (LandChunkSize / 2) * sqrt(1.0f - (float(x)*float(x) / 2) - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) + ((float(x)*float(x)*float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 3));
				x = 1;
				planetPoints_x[1][y + (LandChunkSize / 2)][z + (LandChunkSize / 2)].x = float(x) * sqrt(1.0f - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) + ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 3));
				planetPoints_x[1][y + (LandChunkSize / 2)][z + (LandChunkSize / 2)].y = float(y) / (LandChunkSize / 2) * sqrt(1.0f - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) - (float(x)*float(x) / 2) + ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)*float(x)*float(x)) / 3));
				planetPoints_x[1][y + (LandChunkSize / 2)][z + (LandChunkSize / 2)].z = float(z) / (LandChunkSize / 2) * sqrt(1.0f - (float(x)*float(x) / 2) - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) + ((float(x)*float(x)*float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 3));
			}
		}

		for (int x = -(LandChunkSize / 2); x < (LandChunkSize / 2) + 1; x++) {
			for (int z = -(LandChunkSize / 2); z < (LandChunkSize / 2) + 1; z++) {
				int y = -1;
				planetPoints_y[x + (LandChunkSize / 2)][0][z + (LandChunkSize / 2)].x = float(x) / (LandChunkSize / 2) * sqrt(1.0f - ((float(y)*float(y)) / 2) - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) + ((float(y)*float(y)*float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 3));
				planetPoints_y[x + (LandChunkSize / 2)][0][z + (LandChunkSize / 2)].y = float(y) * sqrt(1.0f - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) + ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 3));
				planetPoints_y[x + (LandChunkSize / 2)][0][z + (LandChunkSize / 2)].z = float(z) / (LandChunkSize / 2) * sqrt(1.0f - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) - ((float(y)*float(y)) / 2) + ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)*float(y)*float(y)) / 3));
				y = 1;
				planetPoints_y[x + (LandChunkSize / 2)][1][z + (LandChunkSize / 2)].x = float(x) / (LandChunkSize / 2) * sqrt(1.0f - ((float(y)*float(y)) / 2) - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) + ((float(y)*float(y)*float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 3));
				planetPoints_y[x + (LandChunkSize / 2)][1][z + (LandChunkSize / 2)].y = float(y) * sqrt(1.0f - ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)) / 2) - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) + ((float(z) / (LandChunkSize / 2)*float(z) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 3));
				planetPoints_y[x + (LandChunkSize / 2)][1][z + (LandChunkSize / 2)].z = float(z) / (LandChunkSize / 2) * sqrt(1.0f - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) - ((float(y)*float(y)) / 2) + ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)*float(y)*float(y)) / 3));
			}
		}

		for (int x = -(LandChunkSize / 2); x < (LandChunkSize / 2) + 1; x++) {
			for (int y = -(LandChunkSize / 2); y < (LandChunkSize / 2) + 1; y++) {
				int z = -1;
				planetPoints_z[x + (LandChunkSize / 2)][y + (LandChunkSize / 2)][0].x = float(x) / (LandChunkSize / 2) * sqrt(1.0f - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) - (float(z)*float(z) / 2) + ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)*float(z)*float(z)) / 3));
				planetPoints_z[x + (LandChunkSize / 2)][y + (LandChunkSize / 2)][0].y = float(y) / (LandChunkSize / 2) * sqrt(1.0f - (float(z)*float(z) / 2) - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) + ((float(z)*float(z)*float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 3));
				planetPoints_z[x + (LandChunkSize / 2)][y + (LandChunkSize / 2)][0].z = float(z) * sqrt(1.0f - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) + ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 3));
				z = 1;
				planetPoints_z[x + (LandChunkSize / 2)][y + (LandChunkSize / 2)][1].x = float(x) / (LandChunkSize / 2) * sqrt(1.0f - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) - (float(z)*float(z) / 2) + ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)*float(z)*float(z)) / 3));
				planetPoints_z[x + (LandChunkSize / 2)][y + (LandChunkSize / 2)][1].y = float(y) / (LandChunkSize / 2) * sqrt(1.0f - (float(z)*float(z) / 2) - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) + ((float(z)*float(z)*float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 3));
				planetPoints_z[x + (LandChunkSize / 2)][y + (LandChunkSize / 2)][1].z = float(z) * sqrt(1.0f - ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)) / 2) - ((float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 2) + ((float(x) / (LandChunkSize / 2)*float(x) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)*float(y) / (LandChunkSize / 2)) / 3));
			}
		}


		counter = 0;
		for (int z = 0; z < (LandChunkSize + 1); z++) {
			for (int x = 0; x < (LandChunkSize + 1); x++) {
				vertices[0][counter].pos = planetPoints_y[x][0][z];
				vertices[0][counter].normal = planetPoints_y[x][0][z];
				vertices[0][counter].texCoord = XMFLOAT2(float(x), float(z));
				vertices[0][counter].biomeHeight = landPoints_y[x][0][z];
				vertices[0][counter].heat = heatPoints_y[x][0][z];
				vertices[0][counter].moisture = moisturePoints_y[x][0][z];
				counter++;
			}
		}
		counter = 0;
		for (int z = 0; z < (LandChunkSize + 1); z++) {
			for (int x = 0; x < (LandChunkSize + 1); x++) {
				vertices[1][counter].pos = planetPoints_y[x][1][z];
				vertices[1][counter].normal = planetPoints_y[x][1][z];
				vertices[1][counter].texCoord = XMFLOAT2(float(x), float(z));
				vertices[1][counter].biomeHeight = landPoints_y[x][1][z];
				vertices[1][counter].heat = heatPoints_y[x][1][z];
				vertices[1][counter].moisture = moisturePoints_y[x][1][z];
				counter++;
			}
		}
		counter = 0;
		for (int y = 0; y < (LandChunkSize + 1); y++) {
			for (int z = 0; z < (LandChunkSize + 1); z++) {
				vertices[2][counter].pos = planetPoints_x[0][y][z];
				vertices[2][counter].normal = planetPoints_x[0][y][z];
				vertices[2][counter].texCoord = XMFLOAT2(float(y), float(z));
				vertices[2][counter].biomeHeight = landPoints_x[0][y][z];
				vertices[2][counter].heat = heatPoints_x[0][y][z];
				vertices[2][counter].moisture = moisturePoints_x[0][y][z];
				counter++;
			}
		}
		counter = 0;
		for (int y = 0; y < (LandChunkSize + 1); y++) {
			for (int z = 0; z < (LandChunkSize + 1); z++) {
				vertices[3][counter].pos = planetPoints_x[1][y][z];
				vertices[3][counter].normal = planetPoints_x[1][y][z];
				vertices[3][counter].texCoord = XMFLOAT2(float(y), float(z));
				vertices[3][counter].biomeHeight = landPoints_x[1][y][z];
				vertices[3][counter].heat = heatPoints_x[1][y][z];
				vertices[3][counter].moisture = moisturePoints_x[1][y][z];
				counter++;
			}
		}
		counter = 0;
		for (int y = 0; y < (LandChunkSize + 1); y++) {
			for (int x = 0; x < (LandChunkSize + 1); x++) {
				vertices[4][counter].pos = planetPoints_z[x][y][0];
				vertices[4][counter].normal = planetPoints_z[x][y][0];
				vertices[4][counter].texCoord = XMFLOAT2(float(x), float(y));
				vertices[4][counter].biomeHeight = landPoints_z[x][y][0];
				vertices[4][counter].heat = heatPoints_z[x][y][0];
				vertices[4][counter].moisture = moisturePoints_z[x][y][0];
				counter++;
			}
		}
		counter = 0;
		for (int y = 0; y < (LandChunkSize + 1); y++) {
			for (int x = 0; x < (LandChunkSize + 1); x++) {
				vertices[5][counter].pos = planetPoints_z[x][y][1];
				vertices[5][counter].normal = planetPoints_z[x][y][1];
				vertices[5][counter].texCoord = XMFLOAT2(float(x), float(y));
				vertices[5][counter].biomeHeight = landPoints_z[x][y][1];
				vertices[5][counter].heat = heatPoints_z[x][y][1];
				vertices[5][counter].moisture = moisturePoints_z[x][y][1];
				counter++;
			}
		}


		counter = 0;
		for (int i = 0; i < 6; i++) {
			for (int z = 0; z < LandChunkSize; z++) {
				for (int x = 0; x < LandChunkSize; x++) {
					indices[i][counter] = x + z*(LandChunkSize + 1);
					counter++;
					indices[i][counter] = x + 1 + z*(LandChunkSize + 1);
					counter++;
					indices[i][counter] = x + (LandChunkSize + 2) + z*(LandChunkSize + 1);
					counter++;
					indices[i][counter] = x + z*(LandChunkSize + 1);
					counter++;
					indices[i][counter] = x + (LandChunkSize + 2) + z*(LandChunkSize + 1);
					counter++;
					indices[i][counter] = x + (LandChunkSize + 1) + z*(LandChunkSize + 1);
					counter++;
				}
			}
		}

		for (int i = 0; i < 6; i++) {
			D3D11_BUFFER_DESC indexBufferDesc;
			ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(indices[i]);
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA iinitData;

			iinitData.pSysMem = indices;
			d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuffer[i]);

			D3D11_BUFFER_DESC vertexBufferDesc;
			ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(vertices[i]);
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData;

			ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
			vertexBufferData.pSysMem = vertices[i];
			hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertBuffer[i]);
		}
	}

	void update() {
		camToPlanet = double3(camPos.x - planetPos.x, camPos.y - planetPos.y, camPos.z - planetPos.z);
		normalizedCamToPlanet = normalize(camToPlanet);

		//Reset cube1World
		groundWorld = XMMatrixIdentity();

		//Define cube1's world space matrix
		Scale = XMMatrixScaling(planetRadius, planetRadius, planetRadius);
		Translation = XMMatrixTranslation(planetPos.x - camPos.x, planetPos.y - camPos.y, planetPos.z - camPos.z);

		//Set cube1's world space using the transformations
		groundWorld = Scale * Translation;

		//Omit futhest face
		for (int i = 0; i < 8; i++)
			drawFace[i] = true;

		if (camPos.x >= 0.0) {
			if (camPos.x / positive(camPos.z) >= 1.0) {
				if (camPos.x / positive(camPos.y) >= 1.0)
					drawFace[2] = false;
			}
		}
		else {
			if (positive(camPos.x) / positive(camPos.z) >= 1.0) {
				if (positive(camPos.x) / positive(camPos.y) >= 1.0)
					drawFace[3] = false;
			}
		}
		if (camPos.y >= 0.0) {
			if (camPos.y / positive(camPos.z) >= 1.0) {
				if (camPos.y / positive(camPos.x) >= 1.0)
					drawFace[0] = false;
			}
		}
		else {
			if (positive(camPos.y) / positive(camPos.z) >= 1.0) {
				if (positive(camPos.y) / positive(camPos.x) >= 1.0)
					drawFace[1] = false;
			}
		}
		if (camPos.z >= 0.0) {
			if (camPos.z / positive(camPos.x) >= 1.0) {
				if (camPos.z / positive(camPos.y) >= 1.0)
					drawFace[4] = false;
			}
		}
		else {
			if (positive(camPos.z) / positive(camPos.x) >= 1.0) {
				if (positive(camPos.z) / positive(camPos.y) >= 1.0)
					drawFace[5] = false;
			}
		}

		for (int i = 0; i < 6; i++) {
			if (drawFace[i] == false) {
				if (float(i) / 2 == int(float(i) / 2)) {// Even number
					closestFace = i + 1;
				}
				else {
					closestFace = i - 1;
				}
			}
		}

		//////////////////////////////////
		cubePos = normalizedCamToPlanet;
		cubizePoint(cubePos);
		//////////////////////////////////

		int counters[6];

		for (int i = 0; i < 6; i++)
			counters[i] = 0;

		for (int n = -2; n < 3; n++) {
			for (int m = -2; m < 3; m++) {
				if (closestFace == 0) {		 // y
					if (floor(128 * cubePos.z) + 128 + n >= 0 && floor(128 * cubePos.z) + 128 + n < 256) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {
							omitIndices[closestFace][counters[closestFace]] = (256 * (floor(128 * cubePos.z) + 128 + n) + floor(cubePos.x * 128) + 128 + m) * 6;
							if (omitIndices[closestFace][counters[closestFace]] != omitIndices[closestFace][counters[closestFace] - 1] || counters[closestFace] == 0) {

								counters[closestFace]++;
							}
						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {
							// keep but do later

							/*
							omitIndices[2][counters[2]] = (256 * (positive(floor(cubePos.x * 128) + 128 + m)-1) + floor(128 * cubePos.z) + 128 + n) * 6;
							if (omitIndices[2][counters[2]] != omitIndices[2][counters[2] - 1] || counters[2] == 0) {

								counters[2]++;
							}
							*/
						}
						else {




						}
					}
					else if (floor(128 * cubePos.z) + 128 + n < 0) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {




						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {



						}
						else {




						}
					}
					else {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {




						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {



						}
						else {

						}
					}
				}

				if (closestFace == 1) {		 // y
					if (floor(128 * cubePos.z) + 128 + n >= 0 && floor(128 * cubePos.z) + 128 + n < 256) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {
							omitIndices[closestFace][counters[closestFace]] = (256 * (floor(128 * cubePos.z) + 128 + n) + floor(cubePos.x * 128) + 128 + m) * 6;
							if (omitIndices[closestFace][counters[closestFace]] != omitIndices[closestFace][counters[closestFace] - 1] || counters[closestFace] == 0) {

								counters[closestFace]++;
							}

						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {

						}
						else {



						}
					}
					else if (floor(128 * cubePos.z) + 128 + n < 0) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {


						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {



						}
						else {


						}
					}
					else {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {


						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {

						}
						else {


						}
					}
				}

				else if (closestFace == 2) { // x
					if (floor(128 * cubePos.y) + 128 + n >= 0 && floor(128 * cubePos.y) + 128 + n < 256) {
						if (floor(128 * cubePos.z) + 128 + m >= 0 && floor(128 * cubePos.z) + 128 + m < 256) {
							omitIndices[closestFace][counters[closestFace]] = (256 * (floor(128 * cubePos.y) + 128 + n) + floor(cubePos.z * 128) + 128 + m) * 6;
							if (omitIndices[closestFace][counters[closestFace]] != omitIndices[closestFace][counters[closestFace] - 1] || counters[closestFace] == 0) {

								counters[closestFace]++;
							}

						}
						else if (floor(128 * cubePos.z) + 128 + m >= 0 && floor(128 * cubePos.z) + 128 + m < 256) {


						}
						else {


						}
					}
					else if (floor(128 * cubePos.y) + 128 + n < 0) {
						if (floor(128 * cubePos.z) + 128 + m >= 0 && floor(128 * cubePos.z) + 128 + m < 256) {


						}
						else if (floor(128 * cubePos.z) + 128 + m < 0) {



						}
						else {

						}
					}
					else {
						if (floor(128 * cubePos.z) + 128 + m >= 0 && floor(128 * cubePos.z) + 128 + m < 256) {



						}
						else if (floor(128 * cubePos.z) + 128 + m < 0) {


						}
						else {



						}
					}
				}

				else if (closestFace == 3) { // x
					if (floor(128 * cubePos.y) + 128 + n >= 0 && floor(128 * cubePos.y) + 128 + n < 256) {
						if (floor(128 * cubePos.z) + 128 + m >= 0 && floor(128 * cubePos.z) + 128 + m < 256) {
							omitIndices[closestFace][counters[closestFace]] = (256 * (floor(128 * cubePos.y) + 128 + n) + floor(cubePos.z * 128) + 128 + m) * 6;
							if (omitIndices[closestFace][counters[closestFace]] != omitIndices[closestFace][counters[closestFace] - 1] || counters[closestFace] == 0) {

								counters[closestFace]++;
							}

						}
						else if (floor(128 * cubePos.z) + 128 + m < 0) {



						}
						else {


						}
					}
					else if (floor(128 * cubePos.y) + 128 + n < 0) {
						if (floor(128 * cubePos.z) + 128 + m >= 0 && floor(128 * cubePos.z) + 128 + m < 256) {



						}
						else if (floor(128 * cubePos.z) + 128 + m < 0) {


						}
						else {


						}
					}
					else {
						if (floor(128 * cubePos.z) + 128 + m >= 0 && floor(128 * cubePos.z) + 128 + m < 256) {



						}
						else if (floor(128 * cubePos.z) + 128 + m < 0) {


						}
						else {


						}
					}
				}

				else if (closestFace == 4) { // z
					if (floor(128 * cubePos.y) + 128 + n >= 0 && floor(128 * cubePos.y) + 128 + n < 256) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {
							omitIndices[closestFace][counters[closestFace]] = (256 * (floor(128 * cubePos.y) + 128 + n) + floor(cubePos.x * 128) + 128 + m) * 6;
							if (omitIndices[closestFace][counters[closestFace]] != omitIndices[closestFace][counters[closestFace] - 1] || counters[closestFace] == 0) {

								counters[closestFace]++;
							}
						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {


						}
						else {


						}
					}
					else if (floor(128 * cubePos.y) + 128 + n < 0) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {


						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {


						}
						else {

						}
					}
					else {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {


						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {


						}
						else {


						}
					}
				}

				else if (closestFace == 5) { // z
					if (floor(128 * cubePos.y) + 128 + n >= 0 && floor(128 * cubePos.y) + 128 + n < 256) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {
							omitIndices[closestFace][counters[closestFace]] = (256 * (floor(128 * cubePos.y) + 128 + n) + floor(cubePos.x * 128) + 128 + m) * 6;
							if (omitIndices[closestFace][counters[closestFace]] != omitIndices[closestFace][counters[closestFace] - 1] || counters[closestFace] == 0) {

								counters[closestFace]++;
							}

						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {



						}
						else {


						}
					}
					else if (floor(128 * cubePos.y) + 128 + n < 0) {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {


						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {


						}
						else {

						}
					}
					else {
						if (floor(128 * cubePos.x) + 128 + m >= 0 && floor(128 * cubePos.x) + 128 + m < 256) {


						}
						else if (floor(128 * cubePos.x) + 128 + m < 0) {


						}
						else {


						}
					}
				}


			}
		}


		for (int i = 0; i < 6; i++) {
			numSquaresToOmit[i] = counters[i];
		}


		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < numSquaresToOmit[i]; j++) {
				for (int k = 0; k < numSquaresToOmit[i] - 1; k++) {

					if (omitIndices[i][k] > omitIndices[i][k + 1]) {
						UINT temp = omitIndices[i][k + 1];
						omitIndices[i][k + 1] = omitIndices[i][k];
						omitIndices[i][k] = temp;
					}

				}
			}
		}






	}

	void draw() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
		d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(planetLayout);

		d3d11DevCon->OMSetDepthStencilState(DSLessEqual, 0);

		for (int i = 0; i < 6; i++) {
			if (drawFace[i] == true) {
				//Set the grounds index buffer
				d3d11DevCon->IASetIndexBuffer(indexBuffer[i], DXGI_FORMAT_R32_UINT, 0);
				//Set the grounds vertex buffer
				UINT stride3 = sizeof(PlanetVertex);
				UINT offset3 = 0;
				d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuffer[i], &stride3, &offset3);

				//Set the WVP matrix and send it to the constant buffer in effect file
				if (vLength(camToPlanet) > 10000000.0)
					WVP = groundWorld * camView * planetCamProjection;
				else
					WVP = groundWorld * camView * planetCamProjectionSmall;
				cbPerObj.WVP = XMMatrixTranspose(WVP);
				cbPerObj.World = XMMatrixTranspose(groundWorld);
				d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
				d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
				d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
				d3d11DevCon->PSSetShaderResources(1, 1, &texture[1]);
				d3d11DevCon->PSSetShaderResources(2, 1, &texture[2]);
				d3d11DevCon->PSSetShaderResources(3, 1, &texture[3]);
				d3d11DevCon->PSSetShaderResources(4, 1, &texture[4]);
				d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

				d3d11DevCon->RSSetState(RSCullNone);
				if (numSquaresToOmit[i] == 0) {
					d3d11DevCon->DrawIndexed(6 * LandChunkSize*LandChunkSize, 0, 0);
				}
				else {
					for (int j = 0; j < numSquaresToOmit[i]; j++) { // number of squares to omit
						if (j == 0)
							d3d11DevCon->DrawIndexed(omitIndices[i][j], 0, 0);
						else
							d3d11DevCon->DrawIndexed(omitIndices[i][j] - (omitIndices[i][j-1]+5) - 1, (omitIndices[i][j-1]+5)+1, 0);

						if (j == numSquaresToOmit[i]-1) { // last square to omit
							d3d11DevCon->DrawIndexed(6 * 256 * 256 - omitIndices[i][j]+5, omitIndices[i][j] + 5 + 1, 0);
						}
					}
				}
			}
		}
	}

	void cleanUp() {
		for (int i = 0; i < 6; i++) {
			vertBuffer[i]->Release();
			indexBuffer[i]->Release();
		}
	}
}planet1;


#endif