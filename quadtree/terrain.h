#pragma once

#include "globals.h"

class TerrainPoint {
public:
	double3 pos;
	double noiseHeight;

	TerrainPoint() {};
	TerrainPoint(double3 pos) {
		double amplitude = 16384.0;
		double maxAmplitude = 0.0;
		double freq = 1.0 / amplitude;
		double lacunarity = 2.0;
		double persistence = 0.5;

		this->pos = pos;
		noiseHeight = 0.0;

		for (int k = 0; k < 22; k++) {
			double t = abs(raw_noise_3d(pos.x * freq + 100000000.0, pos.y * freq + 100000000.0, pos.z * freq + 100000000.0));
			double noise = (raw_noise_3d(pos.x * freq, pos.y * freq, pos.z * freq) + 1.0);
			noiseHeight += noise * amplitude * t; // * t
			maxAmplitude += amplitude;
			amplitude *= persistence;
			freq *= lacunarity;
		}

		noiseHeight /= maxAmplitude;
		noiseHeight += abs(vLength(pos) - maxLength) * (vLength(pos) - maxLength) / 256000000.0;
		// noiseHeight -= 0.15;
	};

	TerrainPoint(double3 pos, double noiseHeight) {
		this->pos = pos;
		this->noiseHeight = noiseHeight;
	}
};