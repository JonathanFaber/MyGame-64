#include "includes.h"

#ifndef __SPRITES_H_INCLUDED__
#define __SPRITES_H_INCLUDED__ 


XMFLOAT2 spriteTexCoord_Num[10];
XMFLOAT2 spriteTexCoord_Box;
XMFLOAT2 spriteTexCoord_FullBox;
XMFLOAT2 spriteTexCoord_Space;


void initSpriteTexCoord(){
	for (int i = 0; i < 10; i++)
		spriteTexCoord_Num[i] = XMFLOAT2(float(i)/16, 0.0f);

	spriteTexCoord_Box = XMFLOAT2(0.0f/16, 0.0f);
	spriteTexCoord_FullBox = XMFLOAT2(1.0f/16, 0.0f);
	spriteTexCoord_Space = XMFLOAT2(2.5f/16, 0.0f);

	counter = 0;
	for (int x = 0; x < 16; x++){
		for (int y = 0; y < 16; y++){



			counter++;
		}
	}

}


class button{
private:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertBuffer;

	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA iinitData;

	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexBufferData; 

	SpriteVertex vertices[4];

	XMMATRIX spriteWorld;

public:
	XMFLOAT2 spritePos;
	XMFLOAT2 spriteScale;
	bool onButton;

	void create(XMFLOAT2 letter_texCoord){
		//Create the vertex buffer

			// Bottom Face
		vertices[0] = SpriteVertex(-0.5f, -0.5f, 0.1f, 0.0f + letter_texCoord.x + 0.0001f, 1.0f/16 + letter_texCoord.y - 0.0001f);
		vertices[1] = SpriteVertex(0.5f, -0.5f, 0.1f, 1.0f/16 + letter_texCoord.x - 0.0001f, 1.0f/16 + letter_texCoord.y - 0.0001f);
		vertices[2] = SpriteVertex(0.5f, 0.5f, 0.1f, 1.0f/16 + letter_texCoord.x - 0.0001f, 0.0f + letter_texCoord.y + 0.0001f);
		vertices[3] = SpriteVertex(-0.5f, 0.5f, 0.1f, 0.0f + letter_texCoord.x + 0.0001f, 0.0f + letter_texCoord.y + 0.0001f);

	//	vertices[0].pos = XMFLOAT3(0.0f, 0.0f, 0.1f);
		//vertices[1].pos = XMFLOAT3(100.0f, 0.0f, 0.1f);
		//vertices[2].pos = XMFLOAT3(100.0f, 100.0f, 0.1f);
		//vertices[3].pos = XMFLOAT3(0.0f, 100.0f, 0.1f);
		

		DWORD indices[6] = {
			0,  1,  2,
			0,  2,  3,
		};

		ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(indices);
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;

		iinitData.pSysMem = indices;
		d3d11Device->CreateBuffer(&indexBufferDesc, &iinitData, &indexBuffer);


		ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof( vertices );
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags =  D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
		vertexBufferData.pSysMem = vertices;
		hr = d3d11Device->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &vertBuffer);
	}

	void update(){
		if(cursorPos.x > spritePos.x - spriteScale.x/2 + float(Width)/2 && cursorPos.x < spritePos.x + spriteScale.x/2 + float(Width)/2){
			if (float(Height)-cursorPos.y > spritePos.y - spriteScale.y/2 + float(Height)/2 && float(Height)-cursorPos.y < spritePos.y + spriteScale.y/2 + float(Height)/2){
				spriteScale.x *= 1.5f;
				spriteScale.y *= 1.5f;
				onButton = true;
			}
			else{
				onButton = false;
			}
		}
		else{
			onButton = false;
		}

		//Reset cube1World
		spriteWorld = XMMatrixIdentity();

		//Define cube1's world space matrix
		Scale = XMMatrixScaling( spriteScale.x, spriteScale.y, 1.0f );
		Translation = XMMatrixTranslation( spritePos.x, spritePos.y, 0.0f );

		//Set cube1's world space using the transformations
		spriteWorld = Scale * Translation;
	}

	void draw(){
		
		//Set Vertex and Pixel Shaders
		d3d11DevCon->VSSetShader(SPRITE_VS, 0, 0);
		d3d11DevCon->GSSetShader(NULL, 0, 0);
		d3d11DevCon->PSSetShader(SPRITE_PS, 0, 0);

		//Set the Input Layout
		d3d11DevCon->IASetInputLayout( spriteLayout );

		//Set the grounds index buffer
		d3d11DevCon->IASetIndexBuffer( indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		UINT stride1 = sizeof( SpriteVertex );
		UINT offset1 = 0;
		d3d11DevCon->IASetVertexBuffers( 0, 1, &vertBuffer, &stride1, &offset1 );

		//Set the WVP matrix and send it to the constant buffer in effect file
		WVP = spriteWorld * spriteView * spriteProjection;
		cbPerObj.WVP = XMMatrixTranspose(WVP);	
		cbPerObj.World = XMMatrixTranspose(spriteWorld);	
		d3d11DevCon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
		d3d11DevCon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
		d3d11DevCon->PSSetShaderResources( 1, 1, &texture[1] );
		d3d11DevCon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

		d3d11DevCon->RSSetState(CCWcullMode);
		d3d11DevCon->DrawIndexed( 6, 0, 0 );
		
	}

	void cleanUp(){
		vertBuffer->Release();
		indexBuffer->Release();
	}

}menuButton_quitWorld, menuButton_resume;

#endif