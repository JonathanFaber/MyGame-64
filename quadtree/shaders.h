#include "shader.h"

#ifndef __SHADERS_H_INCLUDED__
#define __SHADERS_H_INCLUDED__ 

Shader defaultShader, planetShader, modelShader, skymapShader, spriteShader;
ComputeShader computeShader;

void createShaders() {
	defaultShader = Shader(L"default.hlsl", layoutDesc, ARRAYSIZE(layoutDesc));
	planetShader = Shader(L"planet.hlsl", planetLayoutDesc, ARRAYSIZE(planetLayoutDesc), true);
	modelShader = Shader(L"model.hlsl", modelLayoutDesc, ARRAYSIZE(modelLayoutDesc));
	skymapShader = Shader(L"skymap.hlsl", layoutDesc, ARRAYSIZE(layoutDesc));
	spriteShader = Shader(L"sprite.hlsl", spriteLayoutDesc, ARRAYSIZE(spriteLayoutDesc));
	computeShader = ComputeShader(L"compute.hlsl");
}

void cleanUpShaders() {
	defaultShader.cleanUp();
	planetShader.cleanUp();
	modelShader.cleanUp();
	skymapShader.cleanUp();
	spriteShader.cleanUp();
	computeShader.cleanUp();
}

#endif