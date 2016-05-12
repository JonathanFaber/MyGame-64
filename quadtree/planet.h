#include "includes.h"
#include "shaders.h"
#include "simplexnoise.h"


#ifndef __PLANET_H_INCLUDED__
#define __PLANET_H_INCLUDED__ 

const int chunkLength = 32;
const double maxLength = 4194304.0;

class terrainPoint {
public:
	double3 tempNormal;
	double height;
	float height_f;
	double landHeight;
	double temperatureHeight;
	double moistureHeight;
	double height1;
	double landTypeHeight;
	float landTypeHeight_f;
	double amplitude;
	double maxAmplitude;
	double maxAmplitude1;
	double noiseInput[3];
	double freq;
	double lacunarity;
	double persistence;
	double3 terrain;
	double noise[28];

	// change because it sucks
	void generateTerrainPoint(double3 pos) {
		tempNormal = pos;

		pos.x *= maxLength;
		pos.y *= maxLength;
		pos.z *= maxLength;

		height = 0.0;
		height_f = 0.0f;
		landHeight = 0.0;
		height1 = 0.0;
		landTypeHeight = 0.0;
		landTypeHeight_f = 0.0f;
		amplitude;
		maxAmplitude = 0.0;
		maxAmplitude1 = 0.0;
		freq;
		lacunarity = 2.0;
		persistence = 0.5;

		amplitude = 5000.0;
		freq = 1.0;
		for (int k = 0; k < 28; k++) {
			noiseInput[0] = pos.x / maxLength * freq;
			noiseInput[1] = pos.y / maxLength * freq;
			noiseInput[2] = pos.z / maxLength * freq;

			noise[k] = ((raw_noise_3d(noiseInput[0], noiseInput[1], noiseInput[2])));

			freq *= lacunarity;
		}

		maxAmplitude = 0.0;
		amplitude = 5000.0;
		persistence = 0.5;
		for (int k = 6; k < 28; k++) {
			landTypeHeight += ((noise[k])) * amplitude;
			maxAmplitude += amplitude;
			amplitude *= persistence;
		}

		landTypeHeight /= maxAmplitude;
		landTypeHeight_f = float(landTypeHeight);


		double landMaxAmplitude = 0.0;
		amplitude = 5000.0;
		persistence = 0.5;
		for (int k = 0; k < 22; k++) {
			landHeight += ((noise[k])) * amplitude;
			landMaxAmplitude += amplitude;
			amplitude *= persistence;
		}

		landHeight /= landMaxAmplitude;

		landHeight -= 0.15; // because planet is closer to 75 % water

		height_f = float(landHeight);

		if (landHeight > 0.0001) {
			if (landTypeHeight > 0.51) {
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 256.0;
					}
					height += (noise[k]) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;
				height *= maxAmplitude;
			}
			else if (landTypeHeight >= 0.5 && landTypeHeight <= 0.51) {
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 256.0;
					}
					height += (noise[k]) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;
				height *= maxAmplitude;

				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;
				height1 *= maxAmplitude1;

				height = ((landTypeHeight - 0.5) * 100 * height) + (1.0 - ((landTypeHeight - 0.5) * 100)) * height1;
			}
			else if (landTypeHeight > 0.0) {
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude1;
				height -= 0.15;
				height *= maxAmplitude1;
			}
			else if (landTypeHeight >= -0.01 && landTypeHeight <= 0.0) {
				height = 0.0;
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height += ((noise[k])) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;

				height1 = 0.0;
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;

				height = ((landTypeHeight + 0.01) * 100 * height * maxAmplitude) + (1.0 - ((landTypeHeight + 0.01) * 100)) * height1 * maxAmplitude1;
				//height = 1000.0;
			}
			else if (landTypeHeight < -0.01) {
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude1;
				height -= 0.15;
				height *= maxAmplitude1;


				//height = ((landHeight) * 10000 * height1 * maxAmplitude1) + (1.0 - ((landHeight) * 10000)) * landHeight * maxAmplitude;
			}
		}
		else if (landHeight >= 0.0 && landHeight <= 0.0001) {
			if (landTypeHeight > 0.51) {
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 128.0;
					}
					height += ((1.0 - abs(noise[k])) * amplitude);
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;
				height *= maxAmplitude;
				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight >= 0.5 && landTypeHeight <= 0.51) {
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 256.0;
					}
					height += (noise[k]) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;
				height *= maxAmplitude;

				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;
				height1 *= maxAmplitude1;

				height = ((landTypeHeight - 0.5) * 100 * height) + (1.0 - ((landTypeHeight - 0.5) * 100)) * height1;
				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight > 0.0) {
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude1;
				height -= 0.15;
				height *= maxAmplitude1;

				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight >= -0.01 && landTypeHeight <= 0.0) {
				height = 0.0;
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height += ((noise[k])) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;

				height1 = 0.0;
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;

				height = ((landTypeHeight + 0.01) * 100 * height * maxAmplitude) + (1.0 - ((landTypeHeight + 0.01) * 100)) * height1 * maxAmplitude1;
				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight < -0.01) {
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude1;
				height -= 0.15;
				height *= maxAmplitude1;

				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}

		}
		else {
			height = landHeight;
			height *= maxAmplitude;
		}


		if (height <= 0.0)
			height = 0.0;



		terrain = double3(tempNormal.x * height, tempNormal.y * height, tempNormal.z * height);
	}

}terrainPoint;



class Planet {

	struct quad {
		double3 pos;
		double3 posSquare;
		double3 posAway;
		double distance;
		double length;
		bool subdivide;
		bool combine;

		quad() {}
		quad(double3 pos1, double3 posSquare1, double length1) : pos(pos1), posSquare(posSquare1), length(length1) {}

		void update() {
			posAway = double3(pos.x - camPos.x, pos.y - camPos.y, pos.z - camPos.z);
			distance = vLength(posAway);

			if (distance < length*4.0 && length > maxLength / 262144.0) {
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
	};


	class sideY {
		quad quadData[2048];
		bool drawQuad[2048];
		bool firstUpdate[2048];
		int numInstances;

	public:
		double squarePosY;


		void updateQuad() {

			for (int i = 0; i < 2048; i++) {

				if (drawQuad[i] == true) {

					if (quadData[i].subdivide == true) {

						drawQuad[i] = false;
						quad temp = quadData[i];
						counter = 0;

						for (int j = 0; j < 2048; j++) {
							if (drawQuad[j] == false) {
								if (counter == 0) {
									quadData[j].posSquare = double3(temp.posSquare.x - 0.5*temp.length, squarePosY, temp.posSquare.z - 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 1) {
									quadData[j].posSquare = double3(temp.posSquare.x + 0.5*temp.length, squarePosY, temp.posSquare.z - 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);


									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 2) {
									quadData[j].posSquare = double3(temp.posSquare.x + 0.5*temp.length, squarePosY, temp.posSquare.z + 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 3) {
									quadData[j].posSquare = double3(temp.posSquare.x - 0.5*temp.length, squarePosY, temp.posSquare.z + 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
							}
						}
					}
					if (quadData[i].combine == true) {

						if (((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)
							&& ((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)) {

							drawQuad[i] = false;

							for (int j = 0; j < 2048; j++) {
								if (drawQuad[j] == true && quadData[j].length == quadData[i].length) {

									if (quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.z == quadData[j].posSquare.z) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}
									else if (quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}
									else if (quadData[i].posSquare.x == quadData[j].posSquare.x && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}

								}
							}

							quadData[i].posSquare.x += quadData[i].length;
							quadData[i].posSquare.z += quadData[i].length;
							quadData[i].pos = spherize(double3(quadData[i].posSquare.x / maxLength, quadData[i].posSquare.y / maxLength, quadData[i].posSquare.z / maxLength));

							// Next apply height
							// Next apply height
							double3 tempNormal = quadData[i].pos;

							quadData[i].pos.x *= maxLength;
							quadData[i].pos.y *= maxLength;
							quadData[i].pos.z *= maxLength;

							terrainPoint.generateTerrainPoint(tempNormal);



							quadData[i].pos.x += terrainPoint.terrain.x;
							quadData[i].pos.y += terrainPoint.terrain.y;
							quadData[i].pos.z += terrainPoint.terrain.z;

							quadData[i].length *= 2.0;

							quadData[i].combine = false;
							drawQuad[i] = true;
							firstUpdate[i] = true;
						}

					}

				}

			}

		}

		class chunk {
			ID3D11Buffer* indexBuffer;
			ID3D11Buffer* vertBuffer;
			XMMATRIX groundWorld;

			PlanetVertex verticesInitial[(chunkLength + 1)*(chunkLength + 1)];
			PlanetVertex verticesFinal[(chunkLength + 1)*(chunkLength + 1)];
			DWORD indices[chunkLength*chunkLength * 6];

			D3D11_BUFFER_DESC indexBufferDesc;
			D3D11_SUBRESOURCE_DATA indexBufferData;
			D3D11_BUFFER_DESC vertexBufferDesc;
			D3D11_SUBRESOURCE_DATA initialVertexBufferData;
			D3D11_MAPPED_SUBRESOURCE updatedVertexBufferData;

		public:
			double3 firstCamPos;

			void create() {

				counter = 0;
				for (int z = 0; z < chunkLength + 1; z++) {
					for (int x = 0; x < chunkLength + 1; x++) {
						verticesInitial[counter] = PlanetVertex(float(x - chunkLength / 2) / 16.0f, 0.0f, float(z - chunkLength / 2) / 16.0f, float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
						counter++;
					}
				}



				counter = 0;

				for (int x = 0; x < chunkLength; x++) {
					for (int z = 0; z < chunkLength; z++) {
						indices[counter] = x + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + 1 + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 1) + z * (chunkLength + 1);
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

			void update(double3 camPos) {
				groundWorld = XMMatrixIdentity();
				Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
				Translation = XMMatrixTranslation(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
				//Translation = XMMatrixTranslation(0.0f, -5.0f, 0.0f);
				groundWorld = Scale * Translation;
			}

			double3 position[chunkLength + 1][chunkLength + 1];

			void updateChunkData(quad chunkData) {
				for (int z = 0; z < (chunkLength + 1); z++) {
					for (int x = 0; x < (chunkLength + 1); x++) {
						double3 temp = chunkData.posSquare;



						temp.x = double(verticesInitial[z * (chunkLength + 1) + x].pos.x) * chunkData.length;
						temp.y = double(verticesInitial[z * (chunkLength + 1) + x].pos.y) * chunkData.length;
						temp.z = double(verticesInitial[z * (chunkLength + 1) + x].pos.z) * chunkData.length;

						temp.x += chunkData.posSquare.x;
						temp.y += chunkData.posSquare.y;
						temp.z += chunkData.posSquare.z;



						temp.x /= maxLength;
						temp.y /= maxLength;
						temp.z /= maxLength;

						temp = spherize(temp);


						double3 tempNormal = temp;

						temp.x *= maxLength;
						temp.y *= maxLength;
						temp.z *= maxLength;

						terrainPoint.generateTerrainPoint(tempNormal);

						verticesFinal[z * (chunkLength + 1) + x].height = terrainPoint.height_f;
						verticesFinal[z * (chunkLength + 1) + x].landTypeHeight = terrainPoint.landTypeHeight_f;

						temp.x += terrainPoint.terrain.x;
						temp.y += terrainPoint.terrain.y;
						temp.z += terrainPoint.terrain.z;



						temp.x = temp.x - firstCamPos.x;
						temp.y = temp.y - firstCamPos.y;
						temp.z = temp.z - firstCamPos.z;

						position[x][z] = temp;

						verticesFinal[z * (chunkLength + 1) + x].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						verticesFinal[z * (chunkLength + 1) + x].texCoord = XMFLOAT2(float(double(verticesInitial[z * (chunkLength + 1) + x].pos.x) * chunkData.length), float(double(verticesInitial[z * (chunkLength + 1) + x].pos.z) * chunkData.length));
					}
				}

				double3 V;
				double3 W;
				double3 f_normals[2][chunkLength][chunkLength];
				double3 normals[chunkLength + 1][chunkLength + 1];

				counter = 0;
				for (int z = 0; z < chunkLength; z++) {
					for (int x = 0; x < chunkLength; x++) {
						for (int i = 0; i < 2; i++) {
							if (i == 0) {
								V.x = position[x + 1][z + 1].x - position[x][z].x;
								V.y = position[x + 1][z + 1].y - position[x][z].y;
								V.z = position[x + 1][z + 1].z - position[x][z].z;
								W.x = position[x + 1][z].x - position[x][z].x;
								W.y = position[x + 1][z].y - position[x][z].y;
								W.z = position[x + 1][z].z - position[x][z].z;
							}
							if (i == 1) {
								V.x = position[x + 1][z + 1].x - position[x][z].x;
								V.y = position[x + 1][z + 1].y - position[x][z].y;
								V.z = position[x + 1][z + 1].z - position[x][z].z;
								W.x = position[x][z + 1].x - position[x][z].x;
								W.y = position[x][z + 1].y - position[x][z].y;
								W.z = position[x][z + 1].z - position[x][z].z;
							}
							f_normals[i][x][z].x = (V.y*W.z) - (V.z*W.y);
							f_normals[i][x][z].y = (V.z*W.x) - (V.x*W.z);
							f_normals[i][x][z].z = (V.x*W.y) - (V.y*W.x);
							f_normals[i][x][z] = normalize(f_normals[i][x][z]);
							if (chunkData.posSquare.y >= 0.0) {
								if (f_normals[i][x][z].y < 0.0) {
									f_normals[i][x][z].x *= -1;
									f_normals[i][x][z].y *= -1;
									f_normals[i][x][z].z *= -1;
								}
							}
							else if (chunkData.posSquare.y < 0.0) {
								if (f_normals[i][x][z].y > 0.0) {
									f_normals[i][x][z].x *= -1;
									f_normals[i][x][z].y *= -1;
									f_normals[i][x][z].z *= -1;
								}
							}
						}
					}
				}
				//*
				for (int z = 0; z < chunkLength + 1; z++) {
					for (int x = 0; x < chunkLength + 1; x++) {
						if (x != 0 && z != 0 && x != chunkLength && z != chunkLength) {
							normals[x][z].x = ((f_normals[0][x - 1][z - 1].x + f_normals[1][x - 1][z - 1].x) / 2 + f_normals[0][x - 1][z].x + (f_normals[0][x][z].x + f_normals[1][x][z].x) / 2 + f_normals[1][x][z - 1].x) / 4;
							normals[x][z].y = ((f_normals[0][x - 1][z - 1].y + f_normals[1][x - 1][z - 1].y) / 2 + f_normals[0][x - 1][z].y + (f_normals[0][x][z].y + f_normals[1][x][z].y) / 2 + f_normals[1][x][z - 1].y) / 4;
							normals[x][z].z = ((f_normals[0][x - 1][z - 1].z + f_normals[1][x - 1][z - 1].z) / 2 + f_normals[0][x - 1][z].z + (f_normals[0][x][z].z + f_normals[1][x][z].z) / 2 + f_normals[1][x][z - 1].z) / 4;
							normals[x][z] = normalize(normals[x][z]);
						}
						else if (x == chunkLength) {////////////CHANGE//////////////
							normals[x][z].x = f_normals[0][x - 1][z].x + f_normals[1][x - 1][z].x;
							normals[x][z].y = f_normals[0][x - 1][z].y + f_normals[1][x - 1][z].y;
							normals[x][z].z = f_normals[0][x - 1][z].z + f_normals[1][x - 1][z].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						else if (z == chunkLength) {
							normals[x][z].x = f_normals[0][x][z - 1].x + f_normals[1][x][z - 1].x;
							normals[x][z].y = f_normals[0][x][z - 1].y + f_normals[1][x][z - 1].y;
							normals[x][z].z = f_normals[0][x][z - 1].z + f_normals[1][x][z - 1].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						else {////////////CHANGE//////////////
							normals[x][z].x = f_normals[0][x][z].x + f_normals[1][x][z].x;
							normals[x][z].y = f_normals[0][x][z].y + f_normals[1][x][z].y;
							normals[x][z].z = f_normals[0][x][z].z + f_normals[1][x][z].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						verticesFinal[z * (chunkLength + 1) + x].normal = XMFLOAT3(float(normals[x][z].x), float(normals[x][z].y), float(normals[x][z].z));
					}
				}
				//*/
				/*
				for (int z = 0; z < chunkLength + 1; z++) {
					for (int x = 0; x < chunkLength + 1; x++) {
						if (x != chunkLength && z != chunkLength) {
							normals[x][z].x = f_normals[0][x][z].x + f_normals[1][x][z].x;
							normals[x][z].y = f_normals[0][x][z].y + f_normals[1][x][z].y;
							normals[x][z].z = f_normals[0][x][z].z + f_normals[1][x][z].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						else if (x == chunkLength) {////////////CHANGE//////////////
							normals[x][z].x = f_normals[0][x-1][z].x + f_normals[1][x-1][z].x;
							normals[x][z].y = f_normals[0][x-1][z].y + f_normals[1][x-1][z].y;
							normals[x][z].z = f_normals[0][x-1][z].z + f_normals[1][x-1][z].z;
						}
						else {
							normals[x][z].x = f_normals[0][x][z-1].x + f_normals[1][x][z-1].x;
							normals[x][z].y = f_normals[0][x][z-1].y + f_normals[1][x][z-1].x;
							normals[x][z].z = f_normals[0][x][z-1].z + f_normals[1][x][z-1].x;
						}
						verticesFinal[z * (chunkLength + 1) + x].normal = XMFLOAT3(float(normals[x][z].x), float(normals[x][z].y), float(normals[x][z].z));
					}
				}
				//*/

				d3d11DevCon->Map(vertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &updatedVertexBufferData);
				memcpy(updatedVertexBufferData.pData, verticesFinal, sizeof(verticesFinal));
				d3d11DevCon->Unmap(vertBuffer, 0);
			}

			void draw() {
				//Set Vertex and Pixel Shaders
				d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
				d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

				//Set the Input Layout
				d3d11DevCon->IASetInputLayout(planetLayout);

				UINT stride = sizeof(PlanetVertex);
				UINT offset = 0;

				d3d11DevCon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
				d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);

				//Set the WVP matrix and send it to the constant buffer in effect file
				//WVP = groundWorld * camView * camProjection;
				WVP = groundWorld * camView * planetCamProjection;
				cbPerObj.WVP = XMMatrixTranspose(WVP);
				cbPerObj.World = XMMatrixTranspose(groundWorld);
				d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
				d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
				d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
				d3d11DevCon->PSSetShaderResources(1, 1, &texture[2]);
				d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

				d3d11DevCon->RSSetState(RSCullNone);
				//d3d11DevCon->RSSetState(Wireframe);
				d3d11DevCon->DrawIndexed(chunkLength*chunkLength * 6, 0, 0);
			}

			void cleanUp() {
				vertBuffer->Release();
				indexBuffer->Release();
			}
		}sideChunks[2048];

		void create(double squarePos_y) {
			squarePosY = squarePos_y;

			for (int i = 0; i < 2048; i++) {
				drawQuad[i] = false;
				firstUpdate[i] = false;
			}
			drawQuad[0] = true;
			firstUpdate[0] = true;

			quadData[0] = quad(double3(0.0, squarePosY, 0.0), double3(0.0, squarePosY, 0.0), maxLength);
			quadData[0].update();


			for (int i = 0; i < 2048; i++)
				sideChunks[i].create();
		}

		void update(double3 camPos) {
			updateQuad();

			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;




			for (int i = 0; i < 2048; i++) {
				if (drawQuad[i] == true) {
					quadData[i].update();

					if (firstUpdate[i] == true) {
						sideChunks[i].firstCamPos = camPos;
						sideChunks[i].updateChunkData(quadData[i]);

						firstUpdate[i] = false;
					}

					sideChunks[i].update(camPos);
				}
			}
		}

		void draw() {
			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;

			for (int i = 0; i < 2048; i++) {
				if (drawQuad[i] == true && quadData[i].distance < eyeRange) {
					sideChunks[i].draw();
				}
			}
		}

		void cleanUp() {
			for (int i = 0; i < 2048; i++)
				sideChunks[i].cleanUp();
		}

	}sideY0, sideY1;


	class sideX {
		quad quadData[2048];
		bool drawQuad[2048];
		bool firstUpdate[2048];
		int numInstances;

	public:
		double squarePosX;

		void updateQuad() {

			for (int i = 0; i < 2048; i++) {

				if (drawQuad[i] == true) {

					if (quadData[i].subdivide == true) {

						drawQuad[i] = false;
						quad temp = quadData[i];
						counter = 0;

						for (int j = 0; j < 2048; j++) {
							if (drawQuad[j] == false) {
								if (counter == 0) {
									quadData[j].posSquare = double3(squarePosX, temp.posSquare.y - 0.5*temp.length, temp.posSquare.z - 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 1) {
									quadData[j].posSquare = double3(squarePosX, temp.posSquare.y + 0.5*temp.length, temp.posSquare.z - 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 2) {
									quadData[j].posSquare = double3(squarePosX, temp.posSquare.y + 0.5*temp.length, temp.posSquare.z + 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 3) {
									quadData[j].posSquare = double3(squarePosX, temp.posSquare.y - 0.5*temp.length, temp.posSquare.z + 0.5*temp.length);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
							}
						}

					}
					if (quadData[i].combine == true) {

						if (((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)
							&& ((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)) {

							drawQuad[i] = false;

							for (int j = 0; j < 2048; j++) {
								if (drawQuad[j] == true && quadData[j].length == quadData[i].length) {

									if (quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y && quadData[i].posSquare.z == quadData[j].posSquare.z) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}
									else if (quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}
									else if (quadData[i].posSquare.y == quadData[j].posSquare.y && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}

								}
							}

							quadData[i].posSquare.y += quadData[i].length;
							quadData[i].posSquare.z += quadData[i].length;
							quadData[i].pos = spherize(double3(quadData[i].posSquare.x / maxLength, quadData[i].posSquare.y / maxLength, quadData[i].posSquare.z / maxLength));

							// Next apply height
							double3 tempNormal = quadData[i].pos;

							quadData[i].pos.x *= maxLength;
							quadData[i].pos.y *= maxLength;
							quadData[i].pos.z *= maxLength;

							terrainPoint.generateTerrainPoint(tempNormal);



							quadData[i].pos.x += terrainPoint.terrain.x;
							quadData[i].pos.y += terrainPoint.terrain.y;
							quadData[i].pos.z += terrainPoint.terrain.z;

							quadData[i].length *= 2.0;

							quadData[i].combine = false;
							drawQuad[i] = true;
							firstUpdate[i] = true;
						}

					}

				}

			}

		}




		class chunk {
			ID3D11Buffer* indexBuffer;
			ID3D11Buffer* vertBuffer;
			XMMATRIX groundWorld;

			PlanetVertex verticesInitial[(chunkLength + 1)*(chunkLength + 1)];
			PlanetVertex verticesFinal[(chunkLength + 1)*(chunkLength + 1)];
			DWORD indices[chunkLength*chunkLength * 6];

			D3D11_BUFFER_DESC indexBufferDesc;
			D3D11_SUBRESOURCE_DATA indexBufferData;
			D3D11_BUFFER_DESC vertexBufferDesc;
			D3D11_SUBRESOURCE_DATA initialVertexBufferData;
			D3D11_MAPPED_SUBRESOURCE updatedVertexBufferData;

		public:
			double3 firstCamPos;

			void create() {

				counter = 0;
				for (int z = 0; z < chunkLength + 1; z++) {
					for (int x = 0; x < chunkLength + 1; x++) {
						verticesInitial[counter] = PlanetVertex(0.0f, float(z - chunkLength / 2) / 16.0f, float(x - chunkLength / 2) / 16.0f, float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
						counter++;
					}
				}



				counter = 0;

				for (int x = 0; x < chunkLength; x++) {
					for (int z = 0; z < chunkLength; z++) {
						indices[counter] = x + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + 1 + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 1) + z * (chunkLength + 1);
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

			void update(double3 camPos) {
				groundWorld = XMMatrixIdentity();
				Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
				Translation = XMMatrixTranslation(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
				//Translation = XMMatrixTranslation(0.0f, -5.0f, 0.0f);
				groundWorld = Scale * Translation;
			}

			double3 position[chunkLength + 1][chunkLength + 1];

			void updateChunkData(quad chunkData) {
				for (int z = 0; z < (chunkLength + 1); z++) {
					for (int x = 0; x < (chunkLength + 1); x++) {
						double3 temp = chunkData.posSquare;



						temp.x = double(verticesInitial[z * (chunkLength + 1) + x].pos.x) * chunkData.length;
						temp.y = double(verticesInitial[z * (chunkLength + 1) + x].pos.y) * chunkData.length;
						temp.z = double(verticesInitial[z * (chunkLength + 1) + x].pos.z) * chunkData.length;

						temp.x += chunkData.posSquare.x;
						temp.y += chunkData.posSquare.y;
						temp.z += chunkData.posSquare.z;

						temp.x /= maxLength;
						temp.y /= maxLength;
						temp.z /= maxLength;

						temp = spherize(temp);


						double3 tempNormal = temp;

						temp.x *= maxLength;
						temp.y *= maxLength;
						temp.z *= maxLength;

						terrainPoint.generateTerrainPoint(tempNormal);

						verticesFinal[z * (chunkLength + 1) + x].height = terrainPoint.height_f;
						verticesFinal[z * (chunkLength + 1) + x].landTypeHeight = terrainPoint.landTypeHeight_f;

						temp.x += terrainPoint.terrain.x;
						temp.y += terrainPoint.terrain.y;
						temp.z += terrainPoint.terrain.z;

						temp.x = temp.x - firstCamPos.x;
						temp.y = temp.y - firstCamPos.y;
						temp.z = temp.z - firstCamPos.z;

						position[x][z] = temp;

						verticesFinal[z * (chunkLength + 1) + x].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						verticesFinal[z * (chunkLength + 1) + x].texCoord = XMFLOAT2(float(double(verticesInitial[z * (chunkLength + 1) + x].pos.z) * chunkData.length), float(double(verticesInitial[z * (chunkLength + 1) + x].pos.y) * chunkData.length));
					}
				}
				double3 V;
				double3 W;
				double3 f_normals[2][chunkLength][chunkLength];
				double3 normals[chunkLength + 1][chunkLength + 1];

				counter = 0;
				for (int z = 0; z < chunkLength; z++) {
					for (int x = 0; x < chunkLength; x++) {
						for (int i = 0; i < 2; i++) {
							if (i == 0) {
								V.x = position[x + 1][z + 1].x - position[x][z].x;
								V.y = position[x + 1][z + 1].y - position[x][z].y;
								V.z = position[x + 1][z + 1].z - position[x][z].z;
								W.x = position[x + 1][z].x - position[x][z].x;
								W.y = position[x + 1][z].y - position[x][z].y;
								W.z = position[x + 1][z].z - position[x][z].z;
							}
							if (i == 1) {
								V.x = position[x + 1][z + 1].x - position[x][z].x;
								V.y = position[x + 1][z + 1].y - position[x][z].y;
								V.z = position[x + 1][z + 1].z - position[x][z].z;
								W.x = position[x][z + 1].x - position[x][z].x;
								W.y = position[x][z + 1].y - position[x][z].y;
								W.z = position[x][z + 1].z - position[x][z].z;
							}
							f_normals[i][x][z].x = (V.y*W.z) - (V.z*W.y);
							f_normals[i][x][z].y = (V.z*W.x) - (V.x*W.z);
							f_normals[i][x][z].z = (V.x*W.y) - (V.y*W.x);
							f_normals[i][x][z] = normalize(f_normals[i][x][z]);
							if (chunkData.posSquare.x >= 0.0) {
								if (f_normals[i][x][z].x < 0.0) {
									f_normals[i][x][z].x *= -1;
									f_normals[i][x][z].y *= -1;
									f_normals[i][x][z].z *= -1;
								}
							}
							else if (chunkData.posSquare.x < 0.0) {
								if (f_normals[i][x][z].x > 0.0) {
									f_normals[i][x][z].x *= -1;
									f_normals[i][x][z].y *= -1;
									f_normals[i][x][z].z *= -1;
								}
							}
						}
					}
				}
				//*
				for (int z = 0; z < chunkLength + 1; z++) {
					for (int x = 0; x < chunkLength + 1; x++) {
						if (x != 0 && z != 0 && x != chunkLength && z != chunkLength) {
							normals[x][z].x = ((f_normals[0][x - 1][z - 1].x + f_normals[1][x - 1][z - 1].x) / 2 + f_normals[0][x - 1][z].x + (f_normals[0][x][z].x + f_normals[1][x][z].x) / 2 + f_normals[1][x][z - 1].x) / 4;
							normals[x][z].y = ((f_normals[0][x - 1][z - 1].y + f_normals[1][x - 1][z - 1].y) / 2 + f_normals[0][x - 1][z].y + (f_normals[0][x][z].y + f_normals[1][x][z].y) / 2 + f_normals[1][x][z - 1].y) / 4;
							normals[x][z].z = ((f_normals[0][x - 1][z - 1].z + f_normals[1][x - 1][z - 1].z) / 2 + f_normals[0][x - 1][z].z + (f_normals[0][x][z].z + f_normals[1][x][z].z) / 2 + f_normals[1][x][z - 1].z) / 4;
							normals[x][z] = normalize(normals[x][z]);
						}
						else if (x == chunkLength) {////////////CHANGE//////////////
							normals[x][z].x = f_normals[0][x - 1][z].x + f_normals[1][x - 1][z].x;
							normals[x][z].y = f_normals[0][x - 1][z].y + f_normals[1][x - 1][z].y;
							normals[x][z].z = f_normals[0][x - 1][z].z + f_normals[1][x - 1][z].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						else if (z == chunkLength) {
							normals[x][z].x = f_normals[0][x][z - 1].x + f_normals[1][x][z - 1].x;
							normals[x][z].y = f_normals[0][x][z - 1].y + f_normals[1][x][z - 1].y;
							normals[x][z].z = f_normals[0][x][z - 1].z + f_normals[1][x][z - 1].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						else {////////////CHANGE//////////////
							normals[x][z].x = f_normals[0][x][z].x + f_normals[1][x][z].x;
							normals[x][z].y = f_normals[0][x][z].y + f_normals[1][x][z].y;
							normals[x][z].z = f_normals[0][x][z].z + f_normals[1][x][z].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						verticesFinal[z * (chunkLength + 1) + x].normal = XMFLOAT3(float(normals[x][z].x), float(normals[x][z].y), float(normals[x][z].z));
					}
				}


				d3d11DevCon->Map(vertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &updatedVertexBufferData);
				memcpy(updatedVertexBufferData.pData, verticesFinal, sizeof(verticesFinal));
				d3d11DevCon->Unmap(vertBuffer, 0);
			}

			void draw() {
				//Set Vertex and Pixel Shaders
				d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
				d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

				//Set the Input Layout
				d3d11DevCon->IASetInputLayout(planetLayout);

				UINT stride = sizeof(PlanetVertex);
				UINT offset = 0;

				d3d11DevCon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
				d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);

				//Set the WVP matrix and send it to the constant buffer in effect file
				//WVP = groundWorld * camView * camProjection;
				WVP = groundWorld * camView * planetCamProjection;
				cbPerObj.WVP = XMMatrixTranspose(WVP);
				cbPerObj.World = XMMatrixTranspose(groundWorld);
				d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
				d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
				d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
				d3d11DevCon->PSSetShaderResources(1, 1, &texture[2]);
				d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

				d3d11DevCon->RSSetState(RSCullNone);
				//d3d11DevCon->RSSetState(Wireframe);
				d3d11DevCon->DrawIndexed(chunkLength*chunkLength * 6, 0, 0);
			}

			void cleanUp() {
				vertBuffer->Release();
				indexBuffer->Release();
			}
		}sideChunks[2048];

		void create(double squarePos_x) {
			squarePosX = squarePos_x;

			for (int i = 0; i < 2048; i++) {
				drawQuad[i] = false;
				firstUpdate[i] = false;
			}
			drawQuad[0] = true;
			firstUpdate[0] = true;

			quadData[0] = quad(double3(squarePosX, 0.0, 0.0), double3(squarePosX, 0.0, 0.0), maxLength);
			quadData[0].update();


			for (int i = 0; i < 2048; i++)
				sideChunks[i].create();
		}

		void update(double3 camPos) {
			updateQuad();

			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;




			for (int i = 0; i < 2048; i++) {
				if (drawQuad[i] == true) {
					quadData[i].update();

					if (firstUpdate[i] == true) {
						sideChunks[i].firstCamPos = camPos;
						sideChunks[i].updateChunkData(quadData[i]);

						firstUpdate[i] = false;
					}

					sideChunks[i].update(camPos);
				}
			}
		}

		void draw() {
			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;

			for (int i = 0; i < 2048; i++) {
				if (drawQuad[i] == true && quadData[i].distance < eyeRange) {
					sideChunks[i].draw();
				}
			}
		}

		void cleanUp() {
			for (int i = 0; i < 2048; i++)
				sideChunks[i].cleanUp();
		}

	}sideX0, sideX1;


	class sideZ {
		quad quadData[2048];
		bool drawQuad[2048];
		bool firstUpdate[2048];
		int numInstances;

	public:
		double squarePosZ;

		void updateQuad() {

			for (int i = 0; i < 2048; i++) {

				if (drawQuad[i] == true) {

					if (quadData[i].subdivide == true) {

						drawQuad[i] = false;
						quad temp = quadData[i];
						counter = 0;

						for (int j = 0; j < 2048; j++) {
							if (drawQuad[j] == false) {
								if (counter == 0) {
									quadData[j].posSquare = double3(temp.posSquare.x - 0.5*temp.length, temp.posSquare.y - 0.5*temp.length, squarePosZ);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 1) {
									quadData[j].posSquare = double3(temp.posSquare.x + 0.5*temp.length, temp.posSquare.y - 0.5*temp.length, squarePosZ);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 2) {
									quadData[j].posSquare = double3(temp.posSquare.x + 0.5*temp.length, temp.posSquare.y + 0.5*temp.length, squarePosZ);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
								else if (counter == 3) {
									quadData[j].posSquare = double3(temp.posSquare.x - 0.5*temp.length, temp.posSquare.y + 0.5*temp.length, squarePosZ);
									quadData[j].pos = spherize(double3(quadData[j].posSquare.x / maxLength, quadData[j].posSquare.y / maxLength, quadData[j].posSquare.z / maxLength));

									// Next apply height
									double3 tempNormal = quadData[j].pos;

									quadData[j].pos.x *= maxLength;
									quadData[j].pos.y *= maxLength;
									quadData[j].pos.z *= maxLength;
									quadData[j].length = temp.length / 2.0;

									terrainPoint.generateTerrainPoint(tempNormal);



									quadData[j].pos.x += terrainPoint.terrain.x;
									quadData[j].pos.y += terrainPoint.terrain.y;
									quadData[j].pos.z += terrainPoint.terrain.z;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;

									quadData[j].subdivide = false;
									drawQuad[j] = true;
									firstUpdate[j] = true;
									counter++;
								}
							}
						}

					}
					if (quadData[i].combine == true) {

						if (((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)
							&& ((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)) {

							drawQuad[i] = false;

							for (int j = 0; j < 2048; j++) {
								if (drawQuad[j] == true && quadData[j].length == quadData[i].length) {

									if (quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.y == quadData[j].posSquare.y) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}
									else if (quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}
									else if (quadData[i].posSquare.x == quadData[j].posSquare.x && quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y) {
										quadData[j].combine = false;
										drawQuad[j] = false;
									}

								}
							}

							quadData[i].posSquare.x += quadData[i].length;
							quadData[i].posSquare.y += quadData[i].length;
							quadData[i].pos = spherize(double3(quadData[i].posSquare.x / maxLength, quadData[i].posSquare.y / maxLength, quadData[i].posSquare.z / maxLength));

							// Next apply height
							double3 tempNormal = quadData[i].pos;

							quadData[i].pos.x *= maxLength;
							quadData[i].pos.y *= maxLength;
							quadData[i].pos.z *= maxLength;

							terrainPoint.generateTerrainPoint(tempNormal);



							quadData[i].pos.x += terrainPoint.terrain.x;
							quadData[i].pos.y += terrainPoint.terrain.y;
							quadData[i].pos.z += terrainPoint.terrain.z;

							quadData[i].length *= 2.0;

							quadData[i].combine = false;
							drawQuad[i] = true;
							firstUpdate[i] = true;
						}

					}

				}

			}

		}




		class chunk {
			ID3D11Buffer* indexBuffer;
			ID3D11Buffer* vertBuffer;
			XMMATRIX groundWorld;

			PlanetVertex verticesInitial[(chunkLength + 1)*(chunkLength + 1)];
			PlanetVertex verticesFinal[(chunkLength + 1)*(chunkLength + 1)];
			DWORD indices[chunkLength*chunkLength * 6];

			D3D11_BUFFER_DESC indexBufferDesc;
			D3D11_SUBRESOURCE_DATA indexBufferData;
			D3D11_BUFFER_DESC vertexBufferDesc;
			D3D11_SUBRESOURCE_DATA initialVertexBufferData;
			D3D11_MAPPED_SUBRESOURCE updatedVertexBufferData;

		public:
			double3 firstCamPos;

			void create() {

				counter = 0;
				for (int z = 0; z < chunkLength + 1; z++) {
					for (int x = 0; x < chunkLength + 1; x++) {
						verticesInitial[counter] = PlanetVertex(float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 0.0f, float(x - chunkLength / 2) / 16.0f, float(z - chunkLength / 2) / 16.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
						counter++;
					}
				}



				counter = 0;

				for (int x = 0; x < chunkLength; x++) {
					for (int z = 0; z < chunkLength; z++) {
						indices[counter] = x + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + 1 + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 2) + z * (chunkLength + 1);
						counter++;
						indices[counter] = x + (chunkLength + 1) + z * (chunkLength + 1);
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

			void update(double3 camPos) {
				groundWorld = XMMatrixIdentity();
				Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
				Translation = XMMatrixTranslation(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
				//Translation = XMMatrixTranslation(0.0f, -5.0f, 0.0f);
				groundWorld = Scale * Translation;
			}

			double3 position[chunkLength + 1][chunkLength + 1];

			void updateChunkData(quad chunkData) {
				for (int z = 0; z < (chunkLength + 1); z++) {
					for (int x = 0; x < (chunkLength + 1); x++) {
						double3 temp = chunkData.posSquare;



						temp.x = double(verticesInitial[z * (chunkLength + 1) + x].pos.x) * chunkData.length;
						temp.y = double(verticesInitial[z * (chunkLength + 1) + x].pos.y) * chunkData.length;
						temp.z = double(verticesInitial[z * (chunkLength + 1) + x].pos.z) * chunkData.length;

						temp.x += chunkData.posSquare.x;
						temp.y += chunkData.posSquare.y;
						temp.z += chunkData.posSquare.z;

						temp.x /= maxLength;
						temp.y /= maxLength;
						temp.z /= maxLength;

						temp = spherize(temp);


						double3 tempNormal = temp;

						temp.x *= maxLength;
						temp.y *= maxLength;
						temp.z *= maxLength;

						terrainPoint.generateTerrainPoint(tempNormal);

						verticesFinal[z * (chunkLength + 1) + x].height = terrainPoint.height_f;
						verticesFinal[z * (chunkLength + 1) + x].landTypeHeight = terrainPoint.landTypeHeight_f;

						temp.x += terrainPoint.terrain.x;
						temp.y += terrainPoint.terrain.y;
						temp.z += terrainPoint.terrain.z;

						temp.x = temp.x - firstCamPos.x;
						temp.y = temp.y - firstCamPos.y;
						temp.z = temp.z - firstCamPos.z;

						position[x][z] = temp;

						verticesFinal[z * (chunkLength + 1) + x].pos = XMFLOAT3(float(temp.x), float(temp.y), float(temp.z));
						verticesFinal[z * (chunkLength + 1) + x].texCoord = XMFLOAT2(float(double(verticesInitial[z * (chunkLength + 1) + x].pos.x) * chunkData.length), float(double(verticesInitial[z * (chunkLength + 1) + x].pos.y) * chunkData.length));
					}
				}

				double3 V;
				double3 W;
				double3 f_normals[2][chunkLength][chunkLength];
				double3 normals[chunkLength + 1][chunkLength + 1];

				counter = 0;
				for (int z = 0; z < chunkLength; z++) {
					for (int x = 0; x < chunkLength; x++) {
						for (int i = 0; i < 2; i++) {
							if (i == 0) {
								V.x = position[x + 1][z + 1].x - position[x][z].x;
								V.y = position[x + 1][z + 1].y - position[x][z].y;
								V.z = position[x + 1][z + 1].z - position[x][z].z;
								W.x = position[x + 1][z].x - position[x][z].x;
								W.y = position[x + 1][z].y - position[x][z].y;
								W.z = position[x + 1][z].z - position[x][z].z;
							}
							if (i == 1) {
								V.x = position[x + 1][z + 1].x - position[x][z].x;
								V.y = position[x + 1][z + 1].y - position[x][z].y;
								V.z = position[x + 1][z + 1].z - position[x][z].z;
								W.x = position[x][z + 1].x - position[x][z].x;
								W.y = position[x][z + 1].y - position[x][z].y;
								W.z = position[x][z + 1].z - position[x][z].z;
							}
							f_normals[i][x][z].x = (V.y*W.z) - (V.z*W.y);
							f_normals[i][x][z].y = (V.z*W.x) - (V.x*W.z);
							f_normals[i][x][z].z = (V.x*W.y) - (V.y*W.x);
							f_normals[i][x][z] = normalize(f_normals[i][x][z]);
							if (chunkData.posSquare.z >= 0.0) {
								if (f_normals[i][x][z].z < 0.0) {
									f_normals[i][x][z].x *= -1;
									f_normals[i][x][z].y *= -1;
									f_normals[i][x][z].z *= -1;
								}
							}
							else if (chunkData.posSquare.z < 0.0) {
								if (f_normals[i][x][z].z > 0.0) {
									f_normals[i][x][z].x *= -1;
									f_normals[i][x][z].y *= -1;
									f_normals[i][x][z].z *= -1;
								}
							}
						}
					}
				}
				//*
				for (int z = 0; z < chunkLength + 1; z++) {
					for (int x = 0; x < chunkLength + 1; x++) {
						if (x != 0 && z != 0 && x != chunkLength && z != chunkLength) {
							normals[x][z].x = ((f_normals[0][x - 1][z - 1].x + f_normals[1][x - 1][z - 1].x) / 2 + f_normals[0][x - 1][z].x + (f_normals[0][x][z].x + f_normals[1][x][z].x) / 2 + f_normals[1][x][z - 1].x) / 4;
							normals[x][z].y = ((f_normals[0][x - 1][z - 1].y + f_normals[1][x - 1][z - 1].y) / 2 + f_normals[0][x - 1][z].y + (f_normals[0][x][z].y + f_normals[1][x][z].y) / 2 + f_normals[1][x][z - 1].y) / 4;
							normals[x][z].z = ((f_normals[0][x - 1][z - 1].z + f_normals[1][x - 1][z - 1].z) / 2 + f_normals[0][x - 1][z].z + (f_normals[0][x][z].z + f_normals[1][x][z].z) / 2 + f_normals[1][x][z - 1].z) / 4;
							normals[x][z] = normalize(normals[x][z]);
						}
						else if (x == chunkLength) {////////////CHANGE//////////////
							normals[x][z].x = f_normals[0][x - 1][z].x + f_normals[1][x - 1][z].x;
							normals[x][z].y = f_normals[0][x - 1][z].y + f_normals[1][x - 1][z].y;
							normals[x][z].z = f_normals[0][x - 1][z].z + f_normals[1][x - 1][z].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						else if (z == chunkLength) {
							normals[x][z].x = f_normals[0][x][z - 1].x + f_normals[1][x][z - 1].x;
							normals[x][z].y = f_normals[0][x][z - 1].y + f_normals[1][x][z - 1].y;
							normals[x][z].z = f_normals[0][x][z - 1].z + f_normals[1][x][z - 1].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						else {////////////CHANGE//////////////
							normals[x][z].x = f_normals[0][x][z].x + f_normals[1][x][z].x;
							normals[x][z].y = f_normals[0][x][z].y + f_normals[1][x][z].y;
							normals[x][z].z = f_normals[0][x][z].z + f_normals[1][x][z].z;
							normals[x][z] = normalize(normals[x][z]);
						}
						verticesFinal[z * (chunkLength + 1) + x].normal = XMFLOAT3(float(normals[x][z].x), float(normals[x][z].y), float(normals[x][z].z));
					}
				}

				d3d11DevCon->Map(vertBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &updatedVertexBufferData);
				memcpy(updatedVertexBufferData.pData, verticesFinal, sizeof(verticesFinal));
				d3d11DevCon->Unmap(vertBuffer, 0);
			}

			void draw() {
				//Set Vertex and Pixel Shaders
				d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
				d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

				//Set the Input Layout
				d3d11DevCon->IASetInputLayout(planetLayout);

				UINT stride = sizeof(PlanetVertex);
				UINT offset = 0;

				d3d11DevCon->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
				d3d11DevCon->IASetVertexBuffers(0, 1, &vertBuffer, &stride, &offset);

				//Set the WVP matrix and send it to the constant buffer in effect file
				//WVP = groundWorld * camView * camProjection;
				WVP = groundWorld * camView * planetCamProjection;
				cbPerObj.WVP = XMMatrixTranspose(WVP);
				cbPerObj.World = XMMatrixTranspose(groundWorld);
				d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
				d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
				d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
				d3d11DevCon->PSSetShaderResources(1, 1, &texture[2]);
				d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

				d3d11DevCon->RSSetState(RSCullNone);
				//d3d11DevCon->RSSetState(Wireframe);
				d3d11DevCon->DrawIndexed(chunkLength*chunkLength * 6, 0, 0);
			}

			void cleanUp() {
				vertBuffer->Release();
				indexBuffer->Release();
			}
		}sideChunks[2048];

		void create(double squarePos_z) {
			squarePosZ = squarePos_z;

			for (int i = 0; i < 2048; i++) {
				drawQuad[i] = false;
				firstUpdate[i] = false;
			}
			drawQuad[0] = true;
			firstUpdate[0] = true;

			quadData[0] = quad(double3(0.0, 0.0, squarePosZ), double3(0.0, 0.0, squarePosZ), maxLength);
			quadData[0].update();


			for (int i = 0; i < 2048; i++)
				sideChunks[i].create();
		}

		void update(double3 camPos) {
			updateQuad();

			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;




			for (int i = 0; i < 2048; i++) {
				if (drawQuad[i] == true) {
					quadData[i].update();

					if (firstUpdate[i] == true) {
						sideChunks[i].firstCamPos = camPos;
						sideChunks[i].updateChunkData(quadData[i]);

						firstUpdate[i] = false;
					}

					sideChunks[i].update(camPos);
				}
			}
		}

		void draw() {
			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;

			for (int i = 0; i < 2048; i++) {
				if (drawQuad[i] == true && quadData[i].distance < eyeRange) {
					sideChunks[i].draw();
				}
			}
		}

		void cleanUp() {
			for (int i = 0; i < 2048; i++)
				sideChunks[i].cleanUp();
		}

	}sideZ0, sideZ1;


public:

	void init() {
		sideY0.create(-maxLength);
		sideY1.create(maxLength);
		sideX0.create(-maxLength);
		sideX1.create(maxLength);
		sideZ0.create(-maxLength);
		sideZ1.create(maxLength);
	}

	void update() {
		sideY0.update(camPos);
		sideY1.update(camPos);
		sideX0.update(camPos);
		sideX1.update(camPos);
		sideZ0.update(camPos);
		sideZ1.update(camPos);
	}

	void draw() {
		sideY0.draw();
		sideY1.draw();
		sideX0.draw();
		sideX1.draw();
		sideZ0.draw();
		sideZ1.draw();
	}

	void cleanUp() {
		sideY0.cleanUp();
		sideY1.cleanUp();
		sideX0.cleanUp();
		sideX1.cleanUp();
		sideZ0.cleanUp();
		sideZ1.cleanUp();
	}
}planet;

#endif

