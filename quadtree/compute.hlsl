#include "noise3dd.hlsli"

struct InputData
{
	double3 pos;
	double length;
};

struct TerrainData
{
	double3 pos;
	double noiseHeight;
};

StructuredBuffer<InputData>  inputData   : register(t0);
RWStructuredBuffer<TerrainData>        outputData          : register(u0);

double generateTerrainPoint(double3 pos) {
	double amplitude = 1.0;
	double maxAmplitude = 0.0;
	double freq = 1.0;
	double lacunarity = 2.0;
	double persistence = 0.5;
	double noiseHeight = 0.0;

	for (int k = 0; k < 22; k++) {
		noiseHeight += cnoise(double3(pos.x * freq, pos.y * freq, pos.z * freq)) * amplitude;
		maxAmplitude += amplitude;
		amplitude *= persistence;
		freq *= lacunarity;
	}

	noiseHeight *= 0.5;

	noiseHeight -= 0.15; // because planet is closer to 75 % water
	
	return noiseHeight;
}

[numthreads(9, 9, 9)]
void CS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	int x = dispatchThreadID.x;
	int y = dispatchThreadID.y;
	int z = dispatchThreadID.z;
	int chunkLength = 8;
	double3 pos = inputData[0].pos;
	double length = inputData[0].length;
	

	// 0.25 is dependent on chunkSize being 8 ( * 0.25 = / (chunkLength / 2) )
	double3 temp = double3((x - chunkLength * 0.5) * 0.25 * length, (y - chunkLength * 0.5) * 0.25 * length, (z - chunkLength * 0.5) * 0.25 * length);
	temp = temp + pos;
	
	TerrainData output;

	output.pos = temp;
	output.noiseHeight = generateTerrainPoint(temp);

	outputData[(chunkLength + 1) * (chunkLength + 1) * z + (chunkLength + 1) * y + x] = output;
}