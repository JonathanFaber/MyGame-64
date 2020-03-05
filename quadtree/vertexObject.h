#pragma once

#include "globals.h"

class VertexObject {
private:
	ID3D11Buffer* vertBuffer;

public:
	VertexObject() {}
	VertexObject(void* vertexData, UINT vertexSize, UINT vertexCount, bool dynamic = false) {
		D3D11_BUFFER_DESC vertexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexBufferData;

		ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
		ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

		vertexBufferDesc.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = vertexSize * vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		vertexBufferDesc.MiscFlags = 0;

		vertexBufferData.pSysMem = vertexData;
		hr = d3d11Device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &vertBuffer);
	}

	ID3D11Buffer** getBuffer() {
		return &vertBuffer;
	}

	~VertexObject() {
		if (vertBuffer != nullptr)
			vertBuffer->Release();
	}
};
