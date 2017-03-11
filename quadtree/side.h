//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_SIDE_H
#define MYGAME_64_SIDE_H

#include "quadtree.h"

class Side {
    Quad *quad;

public:
    Side(char side, double squarePosSide) {
        if (side == 'y')
            quad = new Quad(double3(0.0, squarePosSide, 0.0), double3(0.0, squarePosSide, 0.0), maxLength, side, squarePosSide);
        else if (side == 'x')
            quad = new Quad(double3(squarePosSide, 0.0, 0.0), double3(squarePosSide, 0.0, 0.0), maxLength, side, squarePosSide);
        else
            quad = new Quad(double3(0.0, 0.0, squarePosSide), double3(0.0, 0.0, squarePosSide), maxLength, side, squarePosSide);
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
        quad->drawTerrain(true);
    }

    void drawFar() {
        quad->drawTerrain(false);
    }

	~Side() {
		delete quad;
    }
};

#endif MYGAME_64_SIDE_H
