#include "includes.h"

#ifndef __SHADERS_H_INCLUDED__
#define __SHADERS_H_INCLUDED__ 

ID3D11VertexShader* VS;
ID3D11PixelShader* PS;
ID3D10Blob* VS_Buffer;
ID3D10Blob* PS_Buffer;

ID3D11VertexShader* SKYMAP_VS;
ID3D11PixelShader* SKYMAP_PS;
ID3D10Blob* SKYMAP_VS_Buffer;
ID3D10Blob* SKYMAP_PS_Buffer;

ID3D11VertexShader* SPRITE_VS;
ID3D11PixelShader* SPRITE_PS;
ID3D10Blob* SPRITE_VS_Buffer;
ID3D10Blob* SPRITE_PS_Buffer;

ID3D11VertexShader* PLANET_VS;
ID3D11PixelShader* PLANET_PS;
ID3D10Blob* PLANET_VS_Buffer;
ID3D10Blob* PLANET_PS_Buffer;

ID3D11VertexShader* INSTANCE_VS;
ID3D11PixelShader* INSTANCE_PS;
ID3D10Blob* INSTANCE_VS_Buffer;
ID3D10Blob* INSTANCE_PS_Buffer;

ID3D11InputLayout* vertLayout;
ID3D11InputLayout* spriteLayout;
ID3D11InputLayout* planetLayout;
ID3D11InputLayout* instanceLayout;

void createShaders(){
	//Compile Shaders from shader file
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer, 0, 0);		//Used to be vs_4_0 and ps_4_0
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "PS", "ps_5_0", 0, 0, 0, &PS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "SKYMAP_VS", "vs_5_0", 0, 0, 0, &SKYMAP_VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "SKYMAP_PS", "ps_5_0", 0, 0, 0, &SKYMAP_PS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "SPRITE_VS", "vs_5_0", 0, 0, 0, &SPRITE_VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "SPRITE_PS", "ps_5_0", 0, 0, 0, &SPRITE_PS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "PLANET_VS", "vs_5_0", 0, 0, 0, &PLANET_VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "PLANET_PS", "ps_5_0", 0, 0, 0, &PLANET_PS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "INSTANCE_VS", "vs_5_0", 0, 0, 0, &INSTANCE_VS_Buffer, 0, 0);
	hr = D3DX11CompileFromFile(L"effects1.fx", 0, 0, "INSTANCE_PS", "ps_5_0", 0, 0, 0, &INSTANCE_PS_Buffer, 0, 0);

	//Create the Shader Objects
	hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
	hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);
	hr = d3d11Device->CreateVertexShader(SKYMAP_VS_Buffer->GetBufferPointer(), SKYMAP_VS_Buffer->GetBufferSize(), NULL, &SKYMAP_VS);
	hr = d3d11Device->CreatePixelShader(SKYMAP_PS_Buffer->GetBufferPointer(), SKYMAP_PS_Buffer->GetBufferSize(), NULL, &SKYMAP_PS);
	hr = d3d11Device->CreateVertexShader(SPRITE_VS_Buffer->GetBufferPointer(), SPRITE_VS_Buffer->GetBufferSize(), NULL, &SPRITE_VS);
	hr = d3d11Device->CreatePixelShader(SPRITE_PS_Buffer->GetBufferPointer(), SPRITE_PS_Buffer->GetBufferSize(), NULL, &SPRITE_PS);	
	hr = d3d11Device->CreateVertexShader(PLANET_VS_Buffer->GetBufferPointer(), PLANET_VS_Buffer->GetBufferSize(), NULL, &PLANET_VS);
	hr = d3d11Device->CreatePixelShader(PLANET_PS_Buffer->GetBufferPointer(), PLANET_PS_Buffer->GetBufferSize(), NULL, &PLANET_PS);
	hr = d3d11Device->CreateVertexShader(INSTANCE_VS_Buffer->GetBufferPointer(), INSTANCE_VS_Buffer->GetBufferSize(), NULL, &INSTANCE_VS);
	hr = d3d11Device->CreatePixelShader(INSTANCE_PS_Buffer->GetBufferPointer(), INSTANCE_PS_Buffer->GetBufferSize(), NULL, &INSTANCE_PS);
}

void createInputLayouts(){
	hr = d3d11Device->CreateInputLayout( layout, numElements, VS_Buffer->GetBufferPointer(), 
		VS_Buffer->GetBufferSize(), &vertLayout );
	hr = d3d11Device->CreateInputLayout( spriteLayoutDesc, numSpriteElements, SPRITE_VS_Buffer->GetBufferPointer(), 
		SPRITE_VS_Buffer->GetBufferSize(), &spriteLayout );
	hr = d3d11Device->CreateInputLayout( planetLayoutDesc, numPlanetElements, PLANET_VS_Buffer->GetBufferPointer(), 
		PLANET_VS_Buffer->GetBufferSize(), &planetLayout );
	hr = d3d11Device->CreateInputLayout(instance_layout, numInstanceElements, INSTANCE_VS_Buffer->GetBufferPointer(),
		INSTANCE_VS_Buffer->GetBufferSize(), &instanceLayout);
}

void cleanUpLayouts(){
	vertLayout->Release();
	spriteLayout->Release();
	planetLayout->Release();
	instanceLayout->Release();
}

void cleanUpShaders(){
	VS->Release();
	PS->Release();
	VS_Buffer->Release();
	PS_Buffer->Release();

	SKYMAP_VS->Release();
	SKYMAP_PS->Release();
	SKYMAP_VS_Buffer->Release();
	SKYMAP_PS_Buffer->Release();

	SPRITE_VS->Release();
	SPRITE_PS->Release();
	SPRITE_VS_Buffer->Release();
	SPRITE_PS_Buffer->Release();

	PLANET_VS->Release();
	PLANET_PS->Release();
	PLANET_VS_Buffer->Release();
	PLANET_PS_Buffer->Release();

	INSTANCE_VS->Release();
	INSTANCE_PS->Release();
	INSTANCE_VS_Buffer->Release();
	INSTANCE_PS_Buffer->Release();
}

#endif