#pragma once

#include "includes.h"

class IndexObject {
private:
	ID3D11Buffer* indexBuffer;

public:
	IndexObject() {}
	IndexObject(void* indexData, UINT indexCount) {
		D3D11_BUFFER_DESC indexBufferDesc;
		D3D11_SUBRESOURCE_DATA indexBufferData;

		ZeroMemory(&indexBufferData, sizeof(indexBufferData));
		ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;		
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		indexBufferData.pSysMem = indexData;
		hr = d3d11Device->CreateBuffer(&indexBufferDesc, &indexBufferData, &indexBuffer);
	}

	ID3D11Buffer* getBuffer() {
		return indexBuffer;
	}

	~IndexObject() {
		if (indexBuffer != nullptr)
			indexBuffer->Release();
	}
};
