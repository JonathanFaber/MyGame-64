#include "includes.h"

#ifndef __SHADER_H_INCLUDED__
#define __SHADER_H_INCLUDED__

class ComputeShader {
	ID3D11ComputeShader* CS;
	ID3DBlob* CS_Buffer;
	ID3D11Buffer* inputBuffer;
	ID3D11ShaderResourceView* inputView;
	ID3D11Buffer* outputBuffer;
	ID3D11Buffer* resultBuffer;
	ID3D11UnorderedAccessView* outputUAV;

public:
	ComputeShader() {}
	ComputeShader(const wchar_t fileName[]) {
		hr = D3DX11CompileFromFile(fileName, 0, 0, "CS", "cs_5_0", 0, 0, 0, &CS_Buffer, 0, 0);
		hr = d3d11Device->CreateComputeShader(CS_Buffer->GetBufferPointer(), CS_Buffer->GetBufferSize(), NULL, &CS);
	}

	void setData(void* inputData, UINT inputSize, UINT outputSize, UINT inputCount, UINT outputCount, bool dynamic = false) {
		D3D11_SUBRESOURCE_DATA inputBufferData;

		ZeroMemory(&inputBufferData, sizeof(inputBufferData));
		inputBufferData.pSysMem = inputData;

		// Create a buffer to be bound as Compute Shader input (D3D11_BIND_SHADER_RESOURCE).
		D3D11_BUFFER_DESC inputBufferDesc;
		inputBufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		inputBufferDesc.ByteWidth = inputSize * inputCount;
		inputBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		inputBufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		inputBufferDesc.StructureByteStride = inputSize;
		inputBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		hr = d3d11Device->CreateBuffer(&inputBufferDesc, &inputBufferData, &inputBuffer);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.Flags = 0;
		srvDesc.BufferEx.NumElements = inputCount;

		hr = d3d11Device->CreateShaderResourceView(inputBuffer, &srvDesc, &inputView);

		// (D3D11_BIND_UNORDERED_ACCESS).
		D3D11_BUFFER_DESC outputDesc;
		outputDesc.Usage = D3D11_USAGE_DEFAULT;
		outputDesc.ByteWidth = outputSize * outputCount;
		outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		outputDesc.CPUAccessFlags = 0;
		outputDesc.StructureByteStride = outputSize;
		outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		hr = d3d11Device->CreateBuffer(&outputDesc, 0, &outputBuffer);

		outputDesc.Usage = D3D11_USAGE_STAGING;
		outputDesc.BindFlags = 0;
		outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		hr = d3d11Device->CreateBuffer(&outputDesc, 0, &resultBuffer);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = 0;
		uavDesc.Buffer.NumElements = outputCount;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		hr = d3d11Device->CreateUnorderedAccessView(outputBuffer, &uavDesc, &outputUAV);
	}

	void* runComputation(int3 threadCount) {
		d3d11DevCon->CSSetShader(CS, 0, 0);
		d3d11DevCon->CSSetShaderResources(0, 1, &inputView);
		d3d11DevCon->CSSetUnorderedAccessViews(0, 1, &outputUAV, 0);

		// Dispatch
		d3d11DevCon->Dispatch(threadCount.x, threadCount.y, threadCount.z);

		// Disable Compute Shader
		d3d11DevCon->CSSetShader(NULL, 0, 0);

		d3d11DevCon->CopyResource(resultBuffer, outputBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = d3d11DevCon->Map(resultBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);

		return mappedResource.pData;
	}

	void updateData(void* data, UINT size) {
		D3D11_MAPPED_SUBRESOURCE updatedBufferData;
		ZeroMemory(&updatedBufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

		d3d11DevCon->Map(inputBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &updatedBufferData);
		memcpy(updatedBufferData.pData, data, size);
		d3d11DevCon->Unmap(inputBuffer, 0);
	}

	void cleanUp() {
		CS->Release();
		CS_Buffer->Release();
		inputBuffer->Release();
		outputBuffer->Release();
	}
};

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
