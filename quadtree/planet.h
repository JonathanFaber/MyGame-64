#include "side.h"

#ifndef __PLANET_H_INCLUDED__
#define __PLANET_H_INCLUDED__ 

class Planet {
public:
	SideY sideY0, sideY1;
	SideX sideX0, sideX1;
	SideZ sideZ0, sideZ1;

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