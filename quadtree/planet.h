#include "includes.h"
#include "shaders.h"
#include "terrain.h"


#ifndef __PLANET_H_INCLUDED__
#define __PLANET_H_INCLUDED__ 

class Planet {
public:
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

		void transform(double3 camPos) {
			groundWorld = XMMatrixIdentity();
			Scale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
			Translation = XMMatrixTranslation(float(firstCamPos.x - camPos.x), float(firstCamPos.y - camPos.y), float(firstCamPos.z - camPos.z));
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

			cbPerObj.WVP = XMMatrixTranspose(WVP);
			cbPerObj.World = XMMatrixTranspose(groundWorld);
			d3d11DevCon->UpdateSubresource(cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0);
			d3d11DevCon->VSSetConstantBuffers(0, 1, &cbPerObjectBuffer);
			d3d11DevCon->PSSetShaderResources(0, 1, &texture[0]);
			d3d11DevCon->PSSetShaderResources(1, 1, &texture[2]);
			d3d11DevCon->PSSetShaderResources(2, 1, &texture[6]);
			d3d11DevCon->PSSetSamplers(0, 1, &CubesTexSamplerState);

			d3d11DevCon->RSSetState(RSCullNone);
			//d3d11DevCon->RSSetState(Wireframe);
			d3d11DevCon->DrawIndexed(chunkLength*chunkLength * 6, 0, 0);
		}

		void cleanUp() {
			vertBuffer->Release();
			indexBuffer->Release();
		}
	};

	class Side {
	public:
		bool exitLoop;
		Quad *quadData;
		double squarePosSide;
		char side;

		virtual void makeChild(Quad *child, Quad parent) = 0;
		virtual void makeParent(Quad *parent) = 0;
		virtual void setSide() = 0;

		static void setChildData(Quad *child, Quad parent) {
			child->pos = spherize(child->posSquare / maxLength);

			// Next apply height
			double3 tempNormal = child->pos;

			child->pos = child->pos * maxLength;
			child->length = parent.length / 2.0;

			terrainPoint.generateTerrainPoint(tempNormal);

			child->pos = child->pos + terrainPoint.terrain;

			child->subdivide = false;
			child->combine = false;
			child->draw = true;
			child->firstUpdate = true;
		}

		static void setParent(Quad *parent) {
			parent->pos = spherize(parent->posSquare / maxLength);

			// Next apply height
			double3 tempNormal = parent->pos;

			parent->pos = parent->pos * maxLength;

			terrainPoint.generateTerrainPoint(tempNormal);

			parent->pos = parent->pos + terrainPoint.terrain;

			parent->length *= 2.0;

			parent->subdivide = false;
			parent->combine = false;
			parent->draw = true;
			parent->firstUpdate = true;
			parent->update();
		}

		void subdivideQuad() {
			exitLoop = false;
			for (int h = 0; h < 2; h++) {
				for (int i = 0; i < 2048; i++) {
					if (exitLoop == false) {
						if (quadData[i].subdivide == true) {
							if ((h == 0 && dotProduct(quadData[i].posAway, camDir) > 0.5) || h == 1) {
								quadData[i].draw = false;
								quadData[i].terrainCalculated = false;
								quadData[i].subdivide = false;
								quadData[i].combine = false;
								Quad parent = quadData[i];
								counter = 0;

								for (int j = 0; j < 2048; j++) {
									if (quadData[j].draw == false) {
										makeChild(&quadData[j], parent);
									}
								}
							}
						}
					}
				}
			}
		}

		void combineQuad() {
			for (double k = minLength; k < maxLength; k *= 2.0) {
				for (int i = 0; i < 2048; i++) {
					if (quadData[i].combine == true && quadData[i].length == k) { ///////////////////////// recombine in order from least length to greatest length, update the quadData member that was just changed
						makeParent(&quadData[i]);
					}
				}
			}
		}

		void create(double squarePos) {
			quadData = new Quad[2048];
			squarePosSide = squarePos;

			for (int i = 0; i < 2048; i++) {
				quadData[i].draw = false;
				quadData[i].terrainCalculated = false;
				quadData[i].firstUpdate = false;
			}
			quadData[0].draw = true;
			quadData[0].firstUpdate = true;

			setSide();

			if (side == 'y')
				quadData[0] = Quad(double3(0.0, squarePosSide, 0.0), double3(0.0, squarePosSide, 0.0), maxLength);
			else if (side == 'x')
				quadData[0] = Quad(double3(squarePosSide, 0.0, 0.0), double3(squarePosSide, 0.0, 0.0), maxLength);
			else
				quadData[0] = Quad(double3(0.0, 0.0, squarePosSide), double3(0.0, 0.0, squarePosSide), maxLength);

			quadData[0].update();

			for (int i = 0; i < 2048; i++)
				quadData[i].create(side);
		}

		void update(double3 camPos) {
			subdivideQuad();
			combineQuad();

			for (int i = 0; i < 2048; i++) {
				if (quadData[i].draw == true) {
					quadData[i].update();

					if (quadData[i].firstUpdate == true) {
						quadData[i].firstCamPos = camPos;
						quadData[i].updateChunkData(side);

						quadData[i].firstUpdate = false;
					}

					quadData[i].transform(camPos);
				}
			}
		}

		void drawClose() {
			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;

			for (int i = 0; i < 2048; i++) {
				if (quadData[i].draw == true && quadData[i].distance < eyeRange && quadData[i].distance < 900.0) {
					if (dotProduct(quadData[i].posAway, camDir) > 0.5 || quadData[i].distance < quadData[i].length * 2.0) {
						if (quadData[i].length == minLength)
							d3d11DevCon->GSSetShader(PLANET_GS, 0, 0);
						else
							d3d11DevCon->GSSetShader(NULL, 0, 0);
						quadData[i].drawTerrain();
					}
				}
			}
		}

		void drawFar() {
			double eyeRange = sqrt(positive(camPos.x*camPos.x + camPos.y*camPos.y + camPos.z*camPos.z - maxLength*maxLength))*1.05;
			if (eyeRange < 8000.0)
				eyeRange = 8000.0;

			for (int i = 0; i < 2048; i++) {
				if (quadData[i].draw == true && quadData[i].distance < eyeRange && quadData[i].distance >= 900.0) {
					if (dotProduct(quadData[i].posAway, camDir) > 0.5 || quadData[i].distance < quadData[i].length * 2.0)
						quadData[i].drawTerrain();
				}
			}
		}

		void cleanUp() {
			for (int i = 0; i < 2048; i++)
				quadData[i].cleanUp();

			delete[] quadData;
		}
	};

	class SideY : public Side {
		void setSide() {
			side = 'y';
		}

		void makeChild(Quad *child, Quad parent) {
			if (counter == 0) {
				child->posSquare = double3(parent.posSquare.x - 0.5*parent.length, squarePosSide, parent.posSquare.z - 0.5*parent.length);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 1) {
				child->posSquare = double3(parent.posSquare.x + 0.5*parent.length, squarePosSide, parent.posSquare.z - 0.5*parent.length);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 2) {
				child->posSquare = double3(parent.posSquare.x + 0.5*parent.length, squarePosSide, parent.posSquare.z + 0.5*parent.length);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 3) {
				child->posSquare = double3(parent.posSquare.x - 0.5*parent.length, squarePosSide, parent.posSquare.z + 0.5*parent.length);
				setChildData(child, parent);
				counter++;
				exitLoop = true;
			}
		}

		void makeParent(Quad *parent) {
			if (((parent->posSquare.x - parent->length + maxLength) / (parent->length * 2.0)) / 2.0 == floor(((parent->posSquare.x - parent->length + maxLength) / (parent->length * 2.0)) / 2.0)
				&& ((parent->posSquare.z - parent->length + maxLength) / (parent->length * 2.0)) / 2.0 == floor(((parent->posSquare.z - parent->length + maxLength) / (parent->length * 2.0)) / 2.0)) {

				parent->draw = false;
				parent->terrainCalculated = false;

				for (int j = 0; j < 2048; j++) {
					if (quadData[j].draw == true && quadData[j].length == parent->length) {

						if (parent->posSquare.x + parent->length * 2.0 == quadData[j].posSquare.x && parent->posSquare.z == quadData[j].posSquare.z ||
							parent->posSquare.x + parent->length * 2.0 == quadData[j].posSquare.x && parent->posSquare.z + parent->length * 2.0 == quadData[j].posSquare.z ||
							parent->posSquare.x == quadData[j].posSquare.x && parent->posSquare.z + parent->length * 2.0 == quadData[j].posSquare.z) {

							quadData[j].subdivide = false;
							quadData[j].combine = false;
							quadData[j].draw = false;
							quadData[j].terrainCalculated = false;
						}
					}
				}

				parent->posSquare.x += parent->length;
				parent->posSquare.z += parent->length;
				setParent(parent);
			}
		}
	}sideY0, sideY1;


	class SideX : public Side {
		void setSide() {
			side = 'x';
		}

		void makeChild(Quad *child, Quad parent) {
			if (counter == 0) {
				child->posSquare = double3(squarePosSide, parent.posSquare.y - 0.5*parent.length, parent.posSquare.z - 0.5*parent.length);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 1) {
				child->posSquare = double3(squarePosSide, parent.posSquare.y + 0.5*parent.length, parent.posSquare.z - 0.5*parent.length);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 2) {
				child->posSquare = double3(squarePosSide, parent.posSquare.y + 0.5*parent.length, parent.posSquare.z + 0.5*parent.length);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 3) {
				child->posSquare = double3(squarePosSide, parent.posSquare.y - 0.5*parent.length, parent.posSquare.z + 0.5*parent.length);
				setChildData(child, parent);
				counter++;
				exitLoop = true;
			}
		}

		void makeParent(Quad *parent) {
			if (((parent->posSquare.y - parent->length + maxLength) / (parent->length * 2.0)) / 2.0 == floor(((parent->posSquare.y - parent->length + maxLength) / (parent->length * 2.0)) / 2.0)
				&& ((parent->posSquare.z - parent->length + maxLength) / (parent->length * 2.0)) / 2.0 == floor(((parent->posSquare.z - parent->length + maxLength) / (parent->length * 2.0)) / 2.0)) {

				parent->draw = false;
				parent->terrainCalculated = false;

				for (int j = 0; j < 2048; j++) {
					if (quadData[j].length == parent->length) {

						if (parent->posSquare.y + parent->length * 2.0 == quadData[j].posSquare.y && parent->posSquare.z == quadData[j].posSquare.z ||
							parent->posSquare.y + parent->length * 2.0 == quadData[j].posSquare.y && parent->posSquare.z + parent->length * 2.0 == quadData[j].posSquare.z ||
							parent->posSquare.y == quadData[j].posSquare.y && parent->posSquare.z + parent->length * 2.0 == quadData[j].posSquare.z) {

							quadData[j].combine = false;
							quadData[j].draw = false;
							quadData[j].terrainCalculated = false;
						}
					}
				}

				parent->posSquare.y += parent->length;
				parent->posSquare.z += parent->length;
				setParent(parent);
			}
		}
	}sideX0, sideX1;


	class SideZ : public Side {
		void setSide() {
			side = 'z';
		}

		void makeChild(Quad *child, Quad parent) {
			if (counter == 0) {
				child->posSquare = double3(parent.posSquare.x - 0.5*parent.length, parent.posSquare.y - 0.5*parent.length, squarePosSide);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 1) {
				child->posSquare = double3(parent.posSquare.x + 0.5*parent.length, parent.posSquare.y - 0.5*parent.length, squarePosSide);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 2) {
				child->posSquare = double3(parent.posSquare.x + 0.5*parent.length, parent.posSquare.y + 0.5*parent.length, squarePosSide);
				setChildData(child, parent);
				counter++;
			}
			else if (counter == 3) {
				child->posSquare = double3(parent.posSquare.x - 0.5*parent.length, parent.posSquare.y + 0.5*parent.length, squarePosSide);
				setChildData(child, parent);
				counter++;
				exitLoop = true;
			}
		}

		void makeParent(Quad *parent) {
			if (((parent->posSquare.x - parent->length + maxLength) / (parent->length * 2.0)) / 2.0 == floor(((parent->posSquare.x - parent->length + maxLength) / (parent->length * 2.0)) / 2.0)
				&& ((parent->posSquare.y - parent->length + maxLength) / (parent->length * 2.0)) / 2.0 == floor(((parent->posSquare.y - parent->length + maxLength) / (parent->length * 2.0)) / 2.0)) {

				parent->draw = false;
				parent->terrainCalculated = false;

				for (int j = 0; j < 2048; j++) {
					if (quadData[j].draw == true && quadData[j].length == parent->length) {

						if (parent->posSquare.x + parent->length * 2.0 == quadData[j].posSquare.x && parent->posSquare.y == quadData[j].posSquare.y ||
							parent->posSquare.x + parent->length * 2.0 == quadData[j].posSquare.x && parent->posSquare.y + parent->length * 2.0 == quadData[j].posSquare.y ||
							parent->posSquare.x == quadData[j].posSquare.x && parent->posSquare.y + parent->length * 2.0 == quadData[j].posSquare.y) {

							quadData[j].combine = false;
							quadData[j].draw = false;
							quadData[j].terrainCalculated = false;
						}
					}
				}

				parent->posSquare.x += parent->length;
				parent->posSquare.y += parent->length;
				setParent(parent);
			}
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

	void drawClose() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
		//set gs in drawClose
		d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(planetLayout);

		sideY0.drawClose();
		sideY1.drawClose();
		sideX0.drawClose();
		sideX1.drawClose();
		sideZ0.drawClose();
		sideZ1.drawClose();
	}

	void drawFar() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
		d3d11DevCon->GSSetShader(NULL, 0, 0);
		d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(planetLayout);

		sideY0.drawFar();
		sideY1.drawFar();
		sideX0.drawFar();
		sideX1.drawFar();
		sideZ0.drawFar();
		sideZ1.drawFar();
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
