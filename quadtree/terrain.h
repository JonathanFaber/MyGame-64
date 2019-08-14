#pragma once

#include "includes.h"
#include "simplexnoise.h"
#include "noiseFunctions.h"

class TerrainPoint {
public:
	double3 pos;
	double noiseHeight;

	TerrainPoint() {};
	TerrainPoint(double3 pos) {
		double amplitude = 1000.0;
		double maxAmplitude = 0.0;
		double freq = 1.0 / 256.0;
		double lacunarity = 2.0;
		double persistence = 0.5;

		this->pos = pos;
		noiseHeight = 0.0;

		for (int k = 0; k < 22; k++) {
			noiseHeight += raw_noise_3d(pos.x * freq, pos.y * freq, pos.z * freq) * amplitude;
			maxAmplitude += amplitude;
			amplitude *= persistence;
			freq *= lacunarity;
		}

		// noiseHeight /= maxAmplitude;
		noiseHeight += abs(vLength(pos) - maxLength) * (vLength(pos) - maxLength);
		// noiseHeight -= 0.15;
	};

	TerrainPoint(double3 pos, double noiseHeight) {
		this->pos = pos;
		this->noiseHeight = noiseHeight;
	}
};