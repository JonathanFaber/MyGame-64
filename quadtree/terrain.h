#pragma once

#include "includes.h"
#include "simplexnoise.h"
#include "noiseFunctions.h"
#include "noiseFunctions.h"

class TerrainPoint {
public:
	double3 pos;
	double landHeight;

	// needs improvement
	void generateTerrainPoint(double3 pos) {
		double amplitude;
		double maxAmplitude;
		double noiseInput[3];
		double freq;
		double lacunarity;
		double persistence;
		double noise[28];

		this->pos = pos;

		pos = normalize(pos);

		pos.x *= maxLength;
		pos.y *= maxLength;
		pos.z *= maxLength;

		landHeight = 0.0;
		amplitude;
		maxAmplitude = 0.0;
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

		// height_f = float(landHeight);
		/*
		if (landHeight > 0.0001) {

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
		else {*/
			// height = landHeight;
			// height *= maxAmplitude;
		//}


		// if (height <= 0.0)
		//	height = 0.0;
	}

};