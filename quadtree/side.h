//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_SIDE_H
#define MYGAME_64_SIDE_H

#include "quadtree.h"

class Side {
    Quad quad;

    Side(char side, double squarePosSide) {
        if (side == 'y')
            quad = Quad(double3(0.0, squarePosSide, 0.0), double3(0.0, squarePosSide, 0.0), maxLength, side, squarePosSide);
        else if (side == 'x')
            quad = Quad(double3(squarePosSide, 0.0, 0.0), double3(squarePosSide, 0.0, 0.0), maxLength, side, squarePosSide);
        else
            quad = Quad(double3(0.0, 0.0, squarePosSide), double3(0.0, 0.0, squarePosSide), maxLength, side, squarePosSide);
    }

    void update() {
        quad.update();
        quad.transform(camPos);
    }

    void drawClose() {
        quad.drawTerrain(true);
    }

    void drawFar() {
        quad.drawTerrain(false);
    }
};

#endif MYGAME_64_SIDE_H
