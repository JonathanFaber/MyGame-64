#pragma once

#include "includes.h"
#include "simplexnoise.h"
#include "noiseFunctions.h"

class TerrainPoint {
public:
	double3 tempNormal;
	double height;
	float height_f;
	double landHeight;
	double temperatureHeight;
	double moistureHeight;
	double height1;
	double landTypeHeight;
	float landTypeHeight_f;
	double amplitude;
	double maxAmplitude;
	double maxAmplitude1;
	double noiseInput[3];
	double freq;
	double lacunarity;
	double persistence;
	double3 terrain;
	double noise[28];

	// change because it sucks
	void generateTerrainPoint(double3 pos) {
		tempNormal = pos;

		pos.x *= maxLength;
		pos.y *= maxLength;
		pos.z *= maxLength;

		height = 0.0;
		height_f = 0.0f;
		landHeight = 0.0;
		height1 = 0.0;
		landTypeHeight = 0.0;
		landTypeHeight_f = 0.0f;
		amplitude;
		maxAmplitude = 0.0;
		maxAmplitude1 = 0.0;
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

		maxAmplitude = 0.0;
		amplitude = 5000.0;
		persistence = 0.5;
		for (int k = 6; k < 28; k++) {
			landTypeHeight += ((noise[k])) * amplitude;
			maxAmplitude += amplitude;
			amplitude *= persistence;
		}

		landTypeHeight /= maxAmplitude;
		landTypeHeight_f = float(landTypeHeight);


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

		height_f = float(landHeight);

		if (landHeight > 0.0001) {
			if (landTypeHeight > 0.54) {
				height = RidgedMultifractal(double3(pos.x / maxLength * 512, pos.y / maxLength * 512, pos.z / maxLength * 512), 1.0, 2.0, 22.0, 1.0, 2.0) * 1000.0;

				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 11; k++) {
					height1 += (noise[k]) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude;
				height1 -= 0.15;
				height1 *= maxAmplitude;

				height += height1;
			}
			else if (landTypeHeight >= 0.5 && landTypeHeight <= 0.54) {
				height = RidgedMultifractal(double3(pos.x / maxLength * 512, pos.y / maxLength * 512, pos.z / maxLength * 512), 1.0, 2.0, 22.0, 1.0, 2.0) * 1000.0;

				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 11; k++) {
					height1 += (noise[k]) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude;
				height1 -= 0.15;
				height1 *= maxAmplitude;

				height += height1;

				height1 = 0.0;
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;
				height1 *= maxAmplitude1;

				height = ((landTypeHeight - 0.5) * 25 * height) + (1.0 - ((landTypeHeight - 0.5) * 25)) * height1;
			}
			else if (landTypeHeight > 0.0) {
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
			else if (landTypeHeight >= -0.01 && landTypeHeight <= 0.0) {
				height = 0.0;
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height += ((noise[k])) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;

				height1 = 0.0;
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;

				height = ((landTypeHeight + 0.01) * 100 * height * maxAmplitude) + (1.0 - ((landTypeHeight + 0.01) * 100)) * height1 * maxAmplitude1;
				//height = 1000.0;
			}
			else if (landTypeHeight < -0.01) {
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude1;
				height -= 0.15;
				height *= maxAmplitude1;


				//height = ((landHeight) * 10000 * height1 * maxAmplitude1) + (1.0 - ((landHeight) * 10000)) * landHeight * maxAmplitude;
			}
		}
		else if (landHeight >= 0.0 && landHeight <= 0.0001) {
			if (landTypeHeight > 0.51) {
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 128.0;
					}
					height += ((1.0 - abs(noise[k])) * amplitude);
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;
				height *= maxAmplitude;
				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight >= 0.5 && landTypeHeight <= 0.51) {
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 256.0;
					}
					height += (noise[k]) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;
				height *= maxAmplitude;

				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;
				height1 *= maxAmplitude1;

				height = ((landTypeHeight - 0.5) * 100 * height) + (1.0 - ((landTypeHeight - 0.5) * 100)) * height1;
				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight > 0.0) {
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

				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight >= -0.01 && landTypeHeight <= 0.0) {
				height = 0.0;
				maxAmplitude = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 11) {
						amplitude *= 12.0;
					}
					height += ((noise[k])) * amplitude;
					maxAmplitude += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude;
				height -= 0.15;

				height1 = 0.0;
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height1 += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height1 /= maxAmplitude1;
				height1 -= 0.15;

				height = ((landTypeHeight + 0.01) * 100 * height * maxAmplitude) + (1.0 - ((landTypeHeight + 0.01) * 100)) * height1 * maxAmplitude1;
				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}
			else if (landTypeHeight < -0.01) {
				maxAmplitude1 = 0.0;
				amplitude = 5000.0;
				persistence = 0.5;
				for (int k = 0; k < 22; k++) {
					if (k == 10)
						amplitude *= 50.0;
					if (k == 15)
						persistence = 0.25;

					height += ((noise[k])) * amplitude;
					maxAmplitude1 += amplitude;
					amplitude *= persistence;
				}

				height /= maxAmplitude1;
				height -= 0.15;
				height *= maxAmplitude1;

				height = ((landHeight) * 10000 * height) + (1.0 - ((landHeight) * 10000)) * landHeight * landMaxAmplitude;
			}

		}
		else {
			height = landHeight;
			height *= maxAmplitude;
		}


		if (height <= 0.0)
			height = 0.0;



		terrain = double3(tempNormal.x * height, tempNormal.y * height, tempNormal.z * height);
	}

}terrainPoint;