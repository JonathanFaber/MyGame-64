#pragma once

#include "includes.h"

IFW1FontWrapper *pFontWrapper;
IFW1Factory *pFW1Factory;

void createText() {
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);

	hResult = pFW1Factory->CreateFontWrapper(d3d11Device, L"Arial", &pFontWrapper);
}

void cleanUpText() {
	pFontWrapper->Release();
	pFW1Factory->Release();
}

class Text {
	std::wstring text;

public:
	void draw(float size, float posX, float posY, UINT32 colour) {
		pFontWrapper->DrawString(
			d3d11DevCon,
			text.c_str(),// String
			size,
			posX,
			posY,
			colour,// Text color, 0xAABBGGRR
			FW1_RESTORESTATE // Flags (for example FW1_RESTORESTATE to keep context states unchanged)
		);
	}

	template <class T>
	void setText(T input) {
		text = boost::lexical_cast<std::wstring>(input);
	}

}FPS, menuText_resume, menuText_quitWorld;;