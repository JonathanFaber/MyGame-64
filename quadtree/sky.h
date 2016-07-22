#include "includes.h"

#ifndef __SKY_H_INCLUDED__
#define __SKY_H_INCLUDED__ 

class sky{
private:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertBuffer;

	int Numvertices;
	int NumSphereFaces;

	XMMATRIX world;

public:
	void CreateSphere(int LatLines, int LongLines)
	{
		Numvertices = ((LatLines-2) * LongLines) + 2;
		NumSphereFaces  = ((LatLines-3)*(LongLines)*2) + (LongLines*2);

		float sphereYaw = 0.0f;
		float spherePitch = 0.0f;

		std::vector<Vertex> vertices(Numvertices);

		XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

		vertices[0].pos.x = 0.0f;
		vertices[0].pos.y = 0.0f;
		vertices[0].pos.z = 1.0f;

		for(DWORD i = 0; i < LatLines-2; ++i)
		{
			spherePitch = (i+1) * (3.14/(LatLines-1));
			Rotationx = XMMatrixRotationX(spherePitch);
			for(DWORD j = 0; j < LongLines; ++j)
			{
				sphereYaw = j * (6.28/(LongLines));
				Rotationy = XMMatrixRotationZ(sphereYaw);
				currVertPos = XMVector3TransformNormal( XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (Rotationx * Rotationy) );	
				currVertPos = XMVector3Normalize( currVertPos );
				vertices[i*LongLines+j+1].pos.x = XMVectorGetX(currVertPos);
				vertices[i*LongLines+j+1].pos.y = XMVectorGetY(currVertPos);
				vertices[i*LongLines+j+1].pos.z = XMVectorGetZ(currVertPos);
			}
		}

		vertices[Numvertices-1].pos.x =  0.0f;
		vertices[Numvertices-1].pos.y =  0.0f;
		vertices[Numvertices-1].pos.z = -1.0f;


		D3D11_BUFFER_DESC vertexBufferDesc;
		ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof( Vertex ) * Numvertices;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexBufferData; 

		ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
		vertexBufferData.pSysMem = &vertices[0];
		hr = d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &vertBuffer);


		std::vector<DWORD> indices(NumSphereFaces * 3);

		int k = 0;
		for(DWORD l = 0; l < LongLines-1; ++l)
		{
			indices[k] = 0;
			indices[k+1] = l+1;
			indices[k+2] = l+2;
			k += 3;
		}

		indices[k] = 0;
		indices[k+1] = LongLines;
		indices[k+2] = 1;
		k += 3;

		for(DWORD i = 0; i < LatLines-3; ++i)
		{
			for(DWORD j = 0; j < LongLines-1; ++j)
			{
				indices[k]   = i*LongLines+j+1;
				indices[k+1] = i*LongLines+j+2;
				indices[k+2] = (i+1)*LongLines+j+1;

				indices[k+3] = (i+1)*LongLines+j+1;
				indices[k+4] = i*LongLines+j+2;
				indices[k+5] = (i+1)*LongLines+j+2;

				k += 6; // next quad
			}

			indices[k]   = (i*LongLines)+LongLines;
			indices[k+1] = (i*LongLines)+1;
			indices[k+2] = ((i+1)*LongLines)+LongLines;

			indices[k+3] = ((i+1)*LongLines)+LongLines;
			indices[k+4] = (i*LongLines)+1;
			indices[k+5] = ((i+1)*LongLines)+1;

			k += 6;
		}

		for(DWORD l = 0; l < LongLines-1; ++l)
		{
			indices[k] = Numvertices-1;
			indices[k+1] = (Numvertices-1)-(l+1);
			indices[k+2] = (Numvertices-1)-(l+2);
			k += 3;
		}

		indices[k] = Numvertices-1;
		indices[k+1] = (Numvertices-1)-LongLines;
		indices[k+2] = Numvertices-2;

		D3D11_BUFFER_DESC indexBufferDesc;
		ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(DWORD) * NumSphereFaces * 3;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA iinitData;

		iinitData.pSysMem = &indices[0];
		d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuffer);

	}

	void updateSphere(){
		//Reset world
		world = XMMatrixIdentity();

		//Define world's world space matrix
		Scale = XMMatrixScaling( 5000000.0f, 5000000.0f, 5000000.0f );
		//Make sure the sphere is always centered around camera
		Translation = XMMatrixTranslation( 0.0f, 0.0f, 0.0f );

		//Set world's world space using the transformations
		world = Scale * Translation;
	}

	void drawSphere(){
		UINT stride2 = sizeof( Vertex );
		UINT offset2 = 0;

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout( vertLayout );

		//Set the spheres index buffer
		d3d11DevCon->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//Set the spheres vertex buffer
		d3d11DevCon->IASetVertexBuffers( 0, 1, &vertBuffer, &stride2, &offset2 );

		//Set the WVP matrix and send it to the constant buffer in effect file
		WVP = world * camView * camProjection[1];
		cbPerObj.WVP = XMMatrixTranspose(WVP);	
		cbPerObj.World = XMMatrixTranspose(world);	
		d3d11DevCon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
		d3d11DevCon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
		//Send our skymap resource view to pixel shader
		d3d11DevCon->PSSetShaderResources( 0, 1, &smrv );
		d3d11DevCon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

		//Set the new VS and PS shaders
		d3d11DevCon->VSSetShader(SKYMAP_VS, 0, 0);
		d3d11DevCon->PSSetShader(SKYMAP_PS, 0, 0);
		//Set the new depth/stencil and RS states
		d3d11DevCon->OMSetDepthStencilState(DSLessEqual, 0);
		d3d11DevCon->RSSetState(RSCullNone);
		d3d11DevCon->DrawIndexed( NumSphereFaces * 3, 0, 0 );
	}

	void cleanUp(){
		indexBuffer->Release();
		vertBuffer->Release();
	}

}skybox;


#endif