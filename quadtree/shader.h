#include "includes.h"

#ifndef __SHADER_H_INCLUDED__
#define __SHADER_H_INCLUDED__

class Shader {
	ID3D11VertexShader* VS;
	ID3D11GeometryShader* GS;
	ID3D11PixelShader* PS;
	ID3DBlob* VS_Buffer;
	ID3DBlob* GS_Buffer;
	ID3DBlob* PS_Buffer;
	ID3D11InputLayout* layout;

public:
	Shader() {}
	Shader(const wchar_t fileName[], D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT layoutSize, bool gs = false) {
		// Compile Shaders from shader file
		hr = D3DX11CompileFromFile(fileName, 0, 0, "VS", "vs_5_0", 0, 0, 0, &VS_Buffer, 0, 0);
		hr = D3DX11CompileFromFile(fileName, 0, 0, "PS", "ps_5_0", 0, 0, 0, &PS_Buffer, 0, 0);

		if (gs)
			hr = D3DX11CompileFromFile(fileName, 0, 0, "GS", "gs_5_0", 0, 0, 0, &GS_Buffer, 0, 0);
		else
			GS_Buffer = nullptr;

		// Create the Shader Objects
		hr = d3d11Device->CreateVertexShader(VS_Buffer->GetBufferPointer(), VS_Buffer->GetBufferSize(), NULL, &VS);
		hr = d3d11Device->CreatePixelShader(PS_Buffer->GetBufferPointer(), PS_Buffer->GetBufferSize(), NULL, &PS);

		if (gs)
			hr = d3d11Device->CreateGeometryShader(GS_Buffer->GetBufferPointer(), GS_Buffer->GetBufferSize(), NULL, &GS);
		else
			GS = nullptr;

		// Create the input layout
		hr = d3d11Device->CreateInputLayout(layoutDesc, layoutSize, VS_Buffer->GetBufferPointer(),
			VS_Buffer->GetBufferSize(), &layout);
	}

	void setShader(bool gs = false) {
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(VS, 0, 0);
		d3d11DevCon->GSSetShader(gs ? GS : NULL, 0, 0);
		d3d11DevCon->PSSetShader(PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout(layout);
	}

	void cleanUp() {
		layout->Release();

		VS->Release();
		PS->Release();

		if (GS != nullptr)
			GS->Release();

		VS_Buffer->Release();
		PS_Buffer->Release();

		if (GS_Buffer != nullptr)
			GS_Buffer->Release();
	}
};


#endif
