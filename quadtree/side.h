//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_SIDE_H
#define MYGAME_64_SIDE_H

#include "quadtree.h"
#include "shaders.h"

class Side {
    Quad *quad;

public:
    Side() {
        quad = new Quad(double3(0.0, maxLength, 0.0), maxLength);
    }

    void update(double3 camPos) {
		int quadsAdded = 0;

        quad->update();
        quad->transform();

		quad->addQuads(&quadsAdded, true);
		quad->addQuads(&quadsAdded, false);
		quad->update();

		for (int l = minLength; l < maxLength; l *= 2.0) {
			quad->removeQuads(l);
			quad->update();
		}
    }

    void drawClose() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
		//set gs in drawClose
		d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(planetLayout);

        quad->drawTerrain(true);
    }

    void drawFar() {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(PLANET_VS, 0, 0);
		d3d11DevCon->GSSetShader(NULL, 0, 0);
		d3d11DevCon->PSSetShader(PLANET_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(planetLayout);

        quad->drawTerrain(false);
    }

	void cleanUp() {
		delete quad;
	}

	~Side() {
		cleanUp();
    }
};

#endif MYGAME_64_SIDE_H
