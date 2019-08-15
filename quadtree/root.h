//
// Created by Jonathan Faber on 2017-02-16.
//

#ifndef MYGAME_64_SIDE_H
#define MYGAME_64_SIDE_H

#include "oct.h"
#include "shaders.h"

class Root {
    Oct *oct;

public:
    Root() {
        oct = new Oct(double3(0.0, 0.0, 0.0), maxLength);
    }

    void update(double3 camPos) {
		int octsAdded = 0;

        oct->update();
        oct->transform();

		oct->addOcts(&octsAdded, true);
		oct->addOcts(&octsAdded, false);
		oct->update();

		for (int l = minLength; l < maxLength; l *= 2.0) {
			oct->removeOcts(l);
			oct->update();
		}
    }

    void drawClose() {
		planetShader.setShader(true);

        oct->drawTerrain(true);
    }

    void drawFar() {
		planetShader.setShader(false);

        oct->drawTerrain(false);
    }

	void cleanUp() {
		delete oct;
	}

	~Root() {
		cleanUp();
    }
};

#endif MYGAME_64_SIDE_H
