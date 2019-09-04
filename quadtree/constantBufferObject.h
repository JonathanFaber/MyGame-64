#pragma once

#include "includes.h"

class ConstantObject {
private:
	ID3D11Buffer* constantBuffer;

public:
	ConstantObject() {}
	ConstantObject(void* constantData, UINT constantSize) {
		D3D11_BUFFER_DESC constantBufferDesc;
		ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));

		constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		constantBufferDesc.ByteWidth = constantSize;
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = 0;
		constantBufferDesc.MiscFlags = 0;

		hr = d3d11Device->CreateBuffer(&constantBufferDesc, NULL, &constantBuffer);
	}

	ID3D11Buffer* getBuffer() {
		return constantBuffer;
	}

	~ConstantObject() {
		if (constantBuffer != nullptr)
			constantBuffer->Release();
	}
};
