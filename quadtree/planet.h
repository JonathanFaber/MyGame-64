#ifndef __PLANET_H_INCLUDED__
#define __PLANET_H_INCLUDED__ 

#include "side.h"

class Planet {
public:
	Side *side[6];

public:
	Planet() {
		side[0] = new Side('y', -maxLength);
		side[1] = new Side('y', maxLength);
		side[2] = new Side('x', -maxLength);
		side[3] = new Side('x', maxLength);
		side[4] = new Side('z', -maxLength);
		side[5] = new Side('z', maxLength);
	}

	void update(double3 camPos) {
		for (int i = 0; i < 6; i++)
			side[i]->update(camPos);
	}

	void drawClose() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
		//set gs in drawClose
		d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(planetLayout);

		for (int i = 0; i < 6; i++)
			side[i]->drawClose();
	}

	void drawFar() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
		d3d11DevCon->GSSetShader(NULL, 0, 0);
		d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(planetLayout);

		for (int i = 0; i < 6; i++)
			side[i]->drawFar();
	}

	void cleanUp() {
		for (int i = 0; i < 6; i++)
			delete side[i];
	}
};

#endif