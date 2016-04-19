#ifndef __INCLUDES_H_INCLUDED__
#define __INCLUDES_H_INCLUDED__ 


//Include and link appropriate libraries and headers//
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

#include <memory>
#include <windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <DirectXMath.h>
#include <sstream>
#include <fstream>
#include <istream>
#include <dinput.h>
#include <vector>
#include "boost/lexical_cast.hpp"
#include <algorithm>
#include <cstdint>
#include <cmath>


using namespace DirectX;


//Global Declarations - Interfaces//
IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;
ID3D11DepthStencilView* depthStencilView;
ID3D11Texture2D* depthStencilBuffer;
ID3D11BlendState* Transparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11RasterizerState* Wireframe;
ID3D11ShaderResourceView* texture[16];
ID3D11SamplerState* CubesTexSamplerState;

ID3D11Buffer* cbPerObjectBuffer;
ID3D11Buffer* cbPerFrameBuffer;

ID3D11Texture2D *BackBuffer11;

IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

ID3D11ShaderResourceView* smrv;

ID3D11DepthStencilState* DSLessEqual;
ID3D11DepthStencilState* DSLessEqualNoDepth;
ID3D11RasterizerState* RSCullNone;

//Global Declarations - Others//
LPCTSTR WndClassName = L"firstwindow";
HWND hwnd = NULL;
HWND desktopHwnd = NULL;
HRESULT hr;

int Width  = 2560;
int Height = 1440;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

float rotx = 0;
float rotz = 0;
float scaleX = 1.0f;
float scaleY = 1.0f;

XMMATRIX Rotationx;
XMMATRIX Rotationy;
XMMATRIX Rotationz;

XMMATRIX WVP;
XMMATRIX camView;
XMMATRIX camProjection;
XMMATRIX planetCamProjection;
XMMATRIX planetCamProjectionSmall;
XMMATRIX starCamProjection;

XMMATRIX spriteView;
XMMATRIX spriteProjection;

XMVECTOR spritePosition;
XMVECTOR spriteTarget;
XMVECTOR spriteUp;


XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

XMFLOAT2 cursorPos;
bool displayCursor = true;
bool leftClick = false;
bool rightClick = false;

bool pressing_ESCAPE = false;

bool resume = false;
bool quit = false;

bool onResume = false;
bool onQuit = false;

bool showGame = true;
bool playingGame = true;
bool showPause = false;

bool windowActive = true;


static const int gradientChunkSize = 64;
static const int numGradients = 2;
XMFLOAT2 gradients[numGradients][numGradients];
int permutation[256];


int counter = 0;

//Function Prototypes//
bool InitializeDirect3d11App(HINSTANCE hInstance);
void CleanUp();
bool InitScene();
void UpdateScene(double time);
void DrawScene();


void StartTimer();
double GetTime();
double GetFrameTime();

bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed);
int messageloop();

bool InitDirectInput(HINSTANCE hInstance);
void DetectInput(double time);

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

//Create effects constant buffer's structure//
struct cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;
};

cbPerObject cbPerObj;
/*
struct cbPerPlanet
{
	XMMATRIX  WVP1;
	XMMATRIX World1;

	UINT excludeIndices[12];
};

cbPerPlanet cbPer_Planet;
*/

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 dir;
	float pad;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

Light light;

struct cbPerFrame
{
	Light  light;
};

cbPerFrame constbuffPerFrame;

struct InstanceData
{
	XMFLOAT2 instancePos;
	float instanceScale;
};


struct Vertex	//Overloaded Vertex Structure
{
	Vertex(){}
	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
};

struct PlanetVertex	//Overloaded Vertex Structure
{
	PlanetVertex(){}
	PlanetVertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz, float h, float htype)
		: pos(x,y,z), texCoord(u, v), normal(nx, ny, nz), height(h), landTypeHeight(htype){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	float height;
	float landTypeHeight;
};

struct SpriteVertex	//Overloaded Vertex Structure
{
	SpriteVertex(){}
	SpriteVertex(float x, float y, float z,
		float u, float v)
		: pos(x,y,z), texCoord(u, v){}

	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
};

D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "NORMAL",	 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

D3D11_INPUT_ELEMENT_DESC planetLayoutDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "HEIGHT",	0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "LANDTYPEHEIGHT",	0, DXGI_FORMAT_R32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

D3D11_INPUT_ELEMENT_DESC spriteLayoutDesc[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
};

D3D11_INPUT_ELEMENT_DESC instance_layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	// Data from the instance buffer
	{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCEPOSBL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCEPOSBR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 24, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCEPOSTR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 36, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCEPOSTL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCEPOSSQUARE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 60, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	{ "INSTANCESCALE", 0, DXGI_FORMAT_R32_FLOAT, 1, 72, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
};

UINT numElements = ARRAYSIZE(layout);
UINT numPlanetElements = ARRAYSIZE(planetLayoutDesc);
UINT numSpriteElements = ARRAYSIZE(spriteLayoutDesc);
UINT numInstanceElements = ARRAYSIZE(instance_layout);



struct double2 {
	double x;
	double y;

	double2() {}
	double2(double x, double y) : x(x), y(y) {}
};

struct double3 {
	double x;
	double y;
	double z;

	double3() {}
	double3(double x, double y, double z) : x(x), y(y), z(z) {}
};

struct int2 {
	int x;
	int y;

	int2() {}
	int2(int x, int y) : x(x), y(y) {}
};

struct int3 {
	int x;
	int y;
	int z;

	int3() {}
	int3(int x, int y, int z) : x(x), y(y), z(z) {}
};

struct vec2 {
	int x;
	int z;

	vec2() {}
	vec2(int x, int z) : x(x), z(z) {}
};


float positive(float num){
	if (num < 0.0f){
		num = -num;
	}
	return num;
}

double positive(double num1) {
	if (num1 < 0.0) {
		num1 = -num1;
	}
	return num1;
}

float negative(float num){
	if (num > 0.0f){
		num = -num;
	}
	return num;
}

float opposite(float num){
	return -num;
}

XMFLOAT3 normalize(XMFLOAT3 vector){
	float length = sqrt(vector.x*vector.x + vector.y*vector.y + vector.z*vector.z);

	if (length != 0.0f){
		vector.x = vector.x/length;
		vector.y = vector.y/length;
		vector.z = vector.z/length;
	}

	return vector;
}

double3 normalize(double3 vector2) {
	double length = sqrt(vector2.x*vector2.x + vector2.y*vector2.y + vector2.z*vector2.z);

	if (length != 0.0) {
		vector2.x = vector2.x / length;
		vector2.y = vector2.y / length;
		vector2.z = vector2.z / length;
	}

	return vector2;
}

XMFLOAT2 normalize(XMFLOAT2 vector1){
	float length1 = sqrt(vector1.x*vector1.x + vector1.y*vector1.y);

	if (length1 != 0.0f){
		vector1.x = vector1.x/length1;
		vector1.y = vector1.y/length1;
	}

	return vector1;
}

double vLength(double3 v) {
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

float dotProduct(XMFLOAT2 a, XMFLOAT2 b){
	return a.x*b.x + a.y*b.y;
}

float dotProduct(XMFLOAT3 a1, XMFLOAT3 b1){
	return a1.x*b1.x + a1.y*b1.y + a1.z*b1.z;
}

inline bool exists(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

double3 spherize(double3 pos) {
	double3 posSquare1;

	posSquare1.x = pos.x * sqrt(1.0 - pos.y*pos.y / 2.0 - pos.z*pos.z / 2.0 + pos.y*pos.y*pos.z*pos.z / 3.0);
	posSquare1.y = pos.y * sqrt(1.0 - pos.x*pos.x / 2.0 - pos.z*pos.z / 2.0 + pos.x*pos.x*pos.z*pos.z / 3.0);
	posSquare1.z = pos.z * sqrt(1.0 - pos.y*pos.y / 2.0 - pos.x*pos.x / 2.0 + pos.y*pos.y*pos.x*pos.x / 3.0);

	return posSquare1;
}


void cubizePoint(double3& position)
{
	double x, y, z;
	x = position.x;
	y = position.y;
	z = position.z;

	double fx, fy, fz;
	fx = positive(x);
	fy = positive(y);
	fz = positive(z);

	const double inverseSqrt2 = 0.70710676908493042;

	if (fy >= fx && fy >= fz) {
		double a2 = x * x * 2.0;
		double b2 = z * z * 2.0;
		double inner = -a2 + b2 - 3;
		double innersqrt = -sqrt((inner * inner) - 12.0 * a2);

		if (x == 0.0) {
			position.x = 0.0;
		}
		else {
			position.x = sqrt(innersqrt + a2 - b2 + 3.0) * inverseSqrt2;
		}

		if (z == 0.0) {
			position.z = 0.0;
		}
		else {
			position.z = sqrt(innersqrt - a2 + b2 + 3.0) * inverseSqrt2;
		}

		if (position.x > 1.0)
			position.x = 1.0;
		if (position.z > 1.0)
			position.z = 1.0;

		if (x < 0)
			position.x = -position.x;
		if (z < 0)
			position.z = -position.z;

		if (y > 0) {
			// top face
			position.y = 1.0;
		}
		else {
			// bottom face
			position.y = -1.0;
		}
	}
	else if (fx >= fy && fx >= fz) {
		double a2 = y * y * 2.0;
		double b2 = z * z * 2.0;
		double inner = -a2 + b2 - 3;
		double innersqrt = -sqrt((inner * inner) - 12.0 * a2);

		if (y == 0.0 || y == -0.0) {
			position.y = 0.0;
		}
		else {
			position.y = sqrt(innersqrt + a2 - b2 + 3.0) * inverseSqrt2;
		}

		if (z == 0.0 || z == -0.0) {
			position.z = 0.0;
		}
		else {
			position.z = sqrt(innersqrt - a2 + b2 + 3.0) * inverseSqrt2;
		}

		if (position.y > 1.0)
			position.y = 1.0;
		if (position.z > 1.0)
			position.z = 1.0;

		if (y < 0)
			position.y = -position.y;
		if (z < 0)
			position.z = -position.z;

		if (x > 0) {
			// right face
			position.x = 1.0;
		}
		else {
			// left face
			position.x = -1.0;
		}
	}
	else {
		double a2 = x * x * 2.0;
		double b2 = y * y * 2.0;
		double inner = -a2 + b2 - 3;
		double innersqrt = -sqrt((inner * inner) - 12.0 * a2);

		if (x == 0.0 || x == -0.0) {
			position.x = 0.0;
		}
		else {
			position.x = sqrt(innersqrt + a2 - b2 + 3.0) * inverseSqrt2;
		}

		if (y == 0.0 || y == -0.0) {
			position.y = 0.0;
		}
		else {
			position.y = sqrt(innersqrt - a2 + b2 + 3.0) * inverseSqrt2;
		}

		if (position.x > 1.0) position.x = 1.0;
		if (position.y > 1.0) position.y = 1.0;

		if (x < 0) position.x = -position.x;
		if (y < 0) position.y = -position.y;

		if (z > 0) {
			// front face
			position.z = 1.0;
		}
		else {
			// back face
			position.z = -1.0;
		}
	}

}


void spherizePoint(double3& position)
{
	double x = position.x;
	double y = position.y;
	double z = position.z;

	position.x = x * sqrt(1.0 - (y*y / 2.0) - (z*z / 2.0) + (y*y*z*z / 3.0));
	position.y = y * sqrt(1.0 - (z*z / 2.0) - (x*x / 2.0) + (z*z*x*x / 3.0));
	position.z = z * sqrt(1.0 - (x*x / 2.0) - (y*y / 2.0) + (x*x*y*y / 3.0));
}


struct osn_context *ctx;

//
double3 camPos = double3(0.0, 5000000.0, 0.0);
//

#endif