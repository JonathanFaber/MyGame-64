#pragma once

#include "quad.h"

class Side {
public:
	bool exitLoop;
	Quad quadData[2048];
	double squarePosSide;
	char side;

	virtual void makeChild(int i, Quad parent) = 0;
	virtual void makeParent(int i) = 0;
	virtual void setSide() = 0;

	void setChildData(int i, Quad parent) {
		quadData[i].pos = spherize(quadData[i].posSquare / maxLength);

		// Next apply height
		double3 tempNormal = quadData[i].pos;

		quadData[i].pos = quadData[i].pos * maxLength;
		quadData[i].length = parent.length / 2.0;

		terrainPoint.generateTerrainPoint(tempNormal);

		quadData[i].pos = quadData[i].pos + terrainPoint.terrain;

		quadData[i].subdivide = false;
		quadData[i].combine = false;
		quadData[i].draw = true;
		quadData[i].firstUpdate = true;
	}

	void setParent(int i) {
		quadData[i].pos = spherize(quadData[i].posSquare / maxLength);

		// Next apply height
		double3 tempNormal = quadData[i].pos;

		quadData[i].pos = quadData[i].pos * maxLength;

		terrainPoint.generateTerrainPoint(tempNormal);

		quadData[i].pos = quadData[i].pos + terrainPoint.terrain;

		quadData[i].length *= 2.0;

		quadData[i].subdivide = false;
		quadData[i].combine = false;
		quadData[i].draw = true;
		quadData[i].firstUpdate = true;
		quadData[i].update();
	}

	void subdivideQuad() {
		exitLoop = false;
		for (int h = 0; h < 2; h++) {
			for (int i = 0; i < 2048; i++) {
				if (quadData[i].subdivide && !exitLoop) {
					if ((h == 0 && dotProduct(quadData[i].posAway, camDir) > 0.5) || h == 1) {
						quadData[i].draw = false;
						quadData[i].terrainCalculated = false;
						quadData[i].subdivide = false;
						quadData[i].combine = false;
						Quad parent = quadData[i];
						counter = 0;

						for (int j = 0; j < 2048; j++) {
							if (!quadData[j].draw) {
								makeChild(j, parent);
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
				if (quadData[i].combine && quadData[i].length == k) { ///////////////////////// recombine in order from least length to greatest length, update the quadData member that was just changed
					makeParent(i);
				}
			}
		}
	}

	void create(double squarePos) {
		//quadData = new Quad[2048];
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
			if (quadData[i].draw) {
				quadData[i].update();

				if (quadData[i].firstUpdate) {
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
			if (quadData[i].draw && quadData[i].distance < eyeRange && quadData[i].distance < 900.0) {
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
			if (quadData[i].draw && quadData[i].distance < eyeRange && quadData[i].distance >= 900.0) {
				if (dotProduct(quadData[i].posAway, camDir) > 0.5 || quadData[i].distance < quadData[i].length * 2.0)
					quadData[i].drawTerrain();
			}
		}
	}

	void cleanUp() {
		for (int i = 0; i < 2048; i++)
			quadData[i].cleanUp();
	}
};

class SideX : public Side {
	void setSide() {
		side = 'x';
	}

	void makeChild(int i, Quad parent) {
		if (counter == 0) {
			quadData[i].posSquare = double3(squarePosSide, parent.posSquare.y - 0.5*parent.length, parent.posSquare.z - 0.5*parent.length);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 1) {
			quadData[i].posSquare = double3(squarePosSide, parent.posSquare.y + 0.5*parent.length, parent.posSquare.z - 0.5*parent.length);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 2) {
			quadData[i].posSquare = double3(squarePosSide, parent.posSquare.y + 0.5*parent.length, parent.posSquare.z + 0.5*parent.length);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 3) {
			quadData[i].posSquare = double3(squarePosSide, parent.posSquare.y - 0.5*parent.length, parent.posSquare.z + 0.5*parent.length);
			setChildData(i, parent);
			counter++;
			exitLoop = true;
		}
	}

	void makeParent(int i) {
		if (((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)
			&& ((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)) {

			quadData[i].draw = false;
			quadData[i].terrainCalculated = false;

			for (int j = 0; j < 2048; j++) {
				if (quadData[j].length == quadData[i].length) {

					if (quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y && quadData[i].posSquare.z == quadData[j].posSquare.z ||
						quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z ||
						quadData[i].posSquare.y == quadData[j].posSquare.y && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z) {

						quadData[j].combine = false;
						quadData[j].draw = false;
						quadData[j].terrainCalculated = false;
					}
				}
			}

			quadData[i].posSquare.y += quadData[i].length;
			quadData[i].posSquare.z += quadData[i].length;
			setParent(i);
		}
	}
};

class SideY : public Side {
	void setSide() {
		side = 'y';
	}

	void makeChild(int i, Quad parent) {
		if (counter == 0) {
			quadData[i].posSquare = double3(parent.posSquare.x - 0.5*parent.length, squarePosSide, parent.posSquare.z - 0.5*parent.length);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 1) {
			quadData[i].posSquare = double3(parent.posSquare.x + 0.5*parent.length, squarePosSide, parent.posSquare.z - 0.5*parent.length);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 2) {
			quadData[i].posSquare = double3(parent.posSquare.x + 0.5*parent.length, squarePosSide, parent.posSquare.z + 0.5*parent.length);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 3) {
			quadData[i].posSquare = double3(parent.posSquare.x - 0.5*parent.length, squarePosSide, parent.posSquare.z + 0.5*parent.length);
			setChildData(i, parent);
			counter++;
			exitLoop = true;
		}
	}

	void makeParent(int i) {
		if (((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)
			&& ((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.z - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)) {

			quadData[i].draw = false;
			quadData[i].terrainCalculated = false;

			for (int j = 0; j < 2048; j++) {
				if (quadData[j].draw == true && quadData[j].length == quadData[i].length) {

					if (quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.z == quadData[j].posSquare.z ||
						quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z ||
						quadData[i].posSquare.x == quadData[j].posSquare.x && quadData[i].posSquare.z + quadData[i].length * 2.0 == quadData[j].posSquare.z) {

						quadData[j].subdivide = false;
						quadData[j].combine = false;
						quadData[j].draw = false;
						quadData[j].terrainCalculated = false;
					}
				}
			}

			quadData[i].posSquare.x += quadData[i].length;
			quadData[i].posSquare.z += quadData[i].length;
			setParent(i);
		}
	}
};

class SideZ : public Side {
	void setSide() {
		side = 'z';
	}

	void makeChild(int i, Quad parent) {
		if (counter == 0) {
			quadData[i].posSquare = double3(parent.posSquare.x - 0.5*parent.length, parent.posSquare.y - 0.5*parent.length, squarePosSide);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 1) {
			quadData[i].posSquare = double3(parent.posSquare.x + 0.5*parent.length, parent.posSquare.y - 0.5*parent.length, squarePosSide);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 2) {
			quadData[i].posSquare = double3(parent.posSquare.x + 0.5*parent.length, parent.posSquare.y + 0.5*parent.length, squarePosSide);
			setChildData(i, parent);
			counter++;
		}
		else if (counter == 3) {
			quadData[i].posSquare = double3(parent.posSquare.x - 0.5*parent.length, parent.posSquare.y + 0.5*parent.length, squarePosSide);
			setChildData(i, parent);
			counter++;
			exitLoop = true;
		}
	}

	void makeParent(int i) {
		if (((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.x - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)
			&& ((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0 == floor(((quadData[i].posSquare.y - quadData[i].length + maxLength) / (quadData[i].length * 2.0)) / 2.0)) {

			quadData[i].draw = false;
			quadData[i].terrainCalculated = false;

			for (int j = 0; j < 2048; j++) {
				if (quadData[j].draw == true && quadData[j].length == quadData[i].length) {

					if (quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.y == quadData[j].posSquare.y ||
						quadData[i].posSquare.x + quadData[i].length * 2.0 == quadData[j].posSquare.x && quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y ||
						quadData[i].posSquare.x == quadData[j].posSquare.x && quadData[i].posSquare.y + quadData[i].length * 2.0 == quadData[j].posSquare.y) {

						quadData[j].combine = false;
						quadData[j].draw = false;
						quadData[j].terrainCalculated = false;
					}
				}
			}

			quadData[i].posSquare.x += quadData[i].length;
			quadData[i].posSquare.y += quadData[i].length;
			setParent(i);
		}
	}
};