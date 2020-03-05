#include "includes.h"

#ifndef __SKY_H_INCLUDED__
#define __SKY_H_INCLUDED__ 

class Sky {
private:
	VertexObject* vertexObj;
	IndexObject* indexObj;

	int nVertices;
	int nSphereFaces;

	XMMATRIX world;

public:
	void CreateSphere(int LatLines, int LongLines)
	{
		nVertices = ((LatLines-2) * LongLines) + 2;
		nSphereFaces  = ((LatLines-3)*(LongLines)*2) + (LongLines*2);

		float sphereYaw = 0.0f;
		float spherePitch = 0.0f;

		std::vector<Vertex> vertices(nVertices);

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

		vertices[nVertices-1].pos.x =  0.0f;
		vertices[nVertices-1].pos.y =  0.0f;
		vertices[nVertices-1].pos.z = -1.0f;


		std::vector<DWORD> indices(nSphereFaces * 3);

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
			indices[k] = nVertices-1;
			indices[k+1] = (nVertices-1)-(l+1);
			indices[k+2] = (nVertices-1)-(l+2);
			k += 3;
		}

		indices[k] = nVertices-1;
		indices[k+1] = (nVertices-1)-LongLines;
		indices[k+2] = nVertices-2;

		vertexObj = new VertexObject(vertices.data(), sizeof(Vertex), nVertices);
		indexObj = new IndexObject(indices.data(), nSphereFaces * 3);
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

		skymapShader.setShader();

		//Set the spheres index buffer
		d3d11DevCon->IASetIndexBuffer(indexObj->getBuffer(), DXGI_FORMAT_R32_UINT, 0);
		//Set the spheres vertex buffer
		d3d11DevCon->IASetVertexBuffers( 0, 1, vertexObj->getBuffer(), &stride2, &offset2 );

		//Set the WVP matrix and send it to the constant buffer in effect file
		WVP = world * camView * camProjection[1];
		cbPerObj.WVP = XMMatrixTranspose(WVP);	
		cbPerObj.World = XMMatrixTranspose(world);	
		d3d11DevCon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
		d3d11DevCon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
		//Send our skymap resource view to pixel shader
		d3d11DevCon->PSSetShaderResources( 0, 1, &smrv );
		d3d11DevCon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

		//Set the new depth/stencil and RS states
		d3d11DevCon->OMSetDepthStencilState(DSLessEqual, 0);
		d3d11DevCon->RSSetState(RSCullNone);
		d3d11DevCon->DrawIndexed(nSphereFaces * 3, 0, 0 );
	}

	~Sky() {
		delete vertexObj;
		delete indexObj;
	}
} skybox;


#endif