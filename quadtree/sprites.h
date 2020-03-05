#include "includes.h"

#ifndef __SPRITES_H_INCLUDED__
#define __SPRITES_H_INCLUDED__ 

XMFLOAT2 spriteTexCoord_Num[10];
XMFLOAT2 spriteTexCoord_Box;
XMFLOAT2 spriteTexCoord_FullBox;
XMFLOAT2 spriteTexCoord_Space;

void initSpriteTexCoord() {
	for (int i = 0; i < 10; i++)
		spriteTexCoord_Num[i] = XMFLOAT2(float(i) / 16, 0.0f);

	spriteTexCoord_Box = XMFLOAT2(0.0f / 16, 0.0f);
	spriteTexCoord_FullBox = XMFLOAT2(1.0f / 16, 0.0f);
	spriteTexCoord_Space = XMFLOAT2(2.5f / 16, 0.0f);

	counter = 0;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {



			counter++;
		}
	}

}

class Button {
private:
	VertexObject* vertexObj;
	IndexObject* indexObj;
	XMMATRIX spriteWorld;

public:
	XMFLOAT2 spritePos;
	XMFLOAT2 spriteScale;
	bool onButton;

	void create(XMFLOAT2 letter_texCoord){
		SpriteVertex vertices[4];

		// Bottom Face
		vertices[0] = SpriteVertex(-0.5f, -0.5f, 0.1f, 0.0f + letter_texCoord.x + 0.0001f, 1.0f/16 + letter_texCoord.y - 0.0001f);
		vertices[1] = SpriteVertex(0.5f, -0.5f, 0.1f, 1.0f/16 + letter_texCoord.x - 0.0001f, 1.0f/16 + letter_texCoord.y - 0.0001f);
		vertices[2] = SpriteVertex(0.5f, 0.5f, 0.1f, 1.0f/16 + letter_texCoord.x - 0.0001f, 0.0f + letter_texCoord.y + 0.0001f);
		vertices[3] = SpriteVertex(-0.5f, 0.5f, 0.1f, 0.0f + letter_texCoord.x + 0.0001f, 0.0f + letter_texCoord.y + 0.0001f);

		//vertices[0].pos = XMFLOAT3(0.0f, 0.0f, 0.1f);
		//vertices[1].pos = XMFLOAT3(100.0f, 0.0f, 0.1f);
		//vertices[2].pos = XMFLOAT3(100.0f, 100.0f, 0.1f);
		//vertices[3].pos = XMFLOAT3(0.0f, 100.0f, 0.1f);
		

		DWORD indices[6] = {
			0,  1,  2,
			0,  2,  3,
		};

		vertexObj = new VertexObject(vertices, sizeof(Vertex), 4, true);
		indexObj = new IndexObject(indices, 6);
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
		spriteShader.setShader();

		//Set the grounds index buffer
		d3d11DevCon->IASetIndexBuffer(indexObj->getBuffer(), DXGI_FORMAT_R32_UINT, 0);
		//Set the grounds vertex buffer
		UINT stride1 = sizeof( SpriteVertex );
		UINT offset1 = 0;
		d3d11DevCon->IASetVertexBuffers(0, 1, vertexObj->getBuffer(), &stride1, &offset1);

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

	~Button() {
		delete vertexObj;
		delete indexObj;
	}
} menuButton_quitWorld, menuButton_resume;

#endif